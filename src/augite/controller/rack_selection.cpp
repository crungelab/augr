#include <augr/core/archive.h>
#include <augr/core/archiver_manufacturer.h>
#include <augr/core/math/vec2.h>
#include <augr/core/model_manufacturer.h>

#include <augr/rack/archiver/graph_archiver.h>
#include <augr/rack/subrack.h>

#include "../widget/module_widget.h"
#include "../widget/widget_builder.h"

#include "../archiver/subrack_view_archiver.h"
#include "../view/subrack_view.h"

#include "rack_selection.h"

namespace augr {

nlohmann::json
RackSelection::BuildSelectionJson(Subrack &subrack, SubrackView &view,
                                  const std::vector<Model *> &modules,
                                  const std::vector<Widget *> &widgets) {
    assert(modules.size() == widgets.size());

    nlohmann::json doc = nlohmann::json::object();
    doc["format"] = "augr.selection";
    doc["version"] = 1;

    // Build "rack" subtree: children + wires, with wire indices local
    // to the selected modules list.
    {
        nlohmann::json rack_json = nlohmann::json::object();
        Archive archive(rack_json);

        // Wrap the raw Model* selection into shared_ptr aliases so we can
        // pass them to SaveChildren/SaveWires which expect vector<Model::Ptr>.
        // No-op deleter — ownership stays with the subrack's children_ list.
        std::vector<Model::Ptr> module_ptrs;
        module_ptrs.reserve(modules.size());
        for (Model *m : modules)
            module_ptrs.push_back(Model::Ptr(m, [](Model *) {}));

        auto &mfr = ArchiverManufacturer::singleton();
        auto *factory = mfr.FindFactory(std::type_index(typeid(subrack)));
        if (factory) {
            std::unique_ptr<Archiver> archiver(factory->Produce(subrack));
            if (auto *graph_arc =
                    dynamic_cast<GraphArchiver *>(archiver.get())) {
                graph_arc->SaveChildren(archive, module_ptrs);
                graph_arc->SaveWires(archive, module_ptrs);
            } else {
                std::cerr
                    << "BuildSelectionJson: archiver is not a GraphArchiver\n";
            }
        } else {
            std::cerr
                << "BuildSelectionJson: no archiver factory for subrack\n";
        }

        doc["rack"] = std::move(rack_json);
    }

    // Build "view" subtree.
    {
        nlohmann::json view_json = nlohmann::json::object();
        Archive archive(view_json);

        auto &mfr = ArchiverManufacturer::singleton();
        auto *factory = mfr.FindFactory(std::type_index(typeid(view)));
        if (factory) {
            std::unique_ptr<Archiver> archiver(factory->Produce(view));
            if (auto *view_arc =
                    dynamic_cast<SubrackViewArchiver *>(archiver.get())) {
                view_arc->SaveWidgets(archive, widgets);
            } else {
                std::cerr << "BuildSelectionJson: archiver is not a "
                             "SubrackViewArchiver\n";
            }
        } else {
            std::cerr << "BuildSelectionJson: no archiver factory for view\n";
        }

        doc["view"] = std::move(view_json);
    }

    // Compute bounding-box origin of the selection for paste anchoring.
    if (!widgets.empty()) {
        bool first = true;
        Vec2 origin{0.0f, 0.0f};
        for (auto *w : widgets) {
            auto *mw = dynamic_cast<ModuleWidget *>(w);
            if (!mw)
                continue;
            if (first) {
                origin = mw->grid_position_;
                first = false;
            } else {
                origin.x = std::min(origin.x, mw->grid_position_.x);
                origin.y = std::min(origin.y, mw->grid_position_.y);
            }
        }
        if (!first)
            doc["origin"] = nlohmann::json::array({origin.x, origin.y});
    }

    return doc;
}

std::vector<Model *>
RackSelection::MergeIntoRack(Subrack &dest, SubrackView &view,
                             const nlohmann::json &selection_json,
                             Vec2 paste_offset) {
    if (selection_json.value("format", "") != "augr.selection")
        return {};
    if (selection_json.value("version", 0) != 1)
        return {};

    nlohmann::json rack_j = selection_json["rack"];
    nlohmann::json view_j = selection_json.contains("view")
                                ? selection_json["view"]
                                : nlohmann::json::object();

    auto &mm = ModelManufacturer::singleton();
    auto &am = ArchiverManufacturer::singleton();

    // --- Load modules ---
    std::vector<Model *> loaded_models;
    if (rack_j.contains("children") && rack_j["children"].is_array()) {
        Archive rack_archive(rack_j);

        for (auto &j_child : rack_j["children"]) {
            if (!j_child.contains("type"))
                continue;
            std::string type_name = j_child["type"].get<std::string>();

            ModelFactory *mf = mm.FindFactory(type_name);
            if (!mf)
                continue;

            auto child_ptr = mf->Produce(dest.shared_from_this());
            Model *child = child_ptr.get();

            JsonScope scope(rack_archive, j_child);
            am.Deserialize(rack_archive, *child);
            loaded_models.push_back(child);
        }
    }

    // --- Bind wires using local indices into loaded_models ---
    if (rack_j.contains("wires") && rack_j["wires"].is_array()) {
        for (const auto &j_wire : rack_j["wires"]) {
            if (!j_wire.is_array() || j_wire.size() != 2)
                continue;
            const auto &j_from = j_wire[0];
            const auto &j_to = j_wire[1];
            if (!j_from.is_array() || j_from.size() != 2)
                continue;
            if (!j_to.is_array() || j_to.size() != 2)
                continue;

            int from_idx = j_from[0].get<int>();
            int to_idx = j_to[0].get<int>();
            std::string from_pin = j_from[1].get<std::string>();
            std::string to_pin = j_to[1].get<std::string>();

            if (from_idx < 0 || from_idx >= (int)loaded_models.size())
                continue;
            if (to_idx < 0 || to_idx >= (int)loaded_models.size())
                continue;

            Node *from_node = dynamic_cast<Node *>(loaded_models[from_idx]);
            Node *to_node = dynamic_cast<Node *>(loaded_models[to_idx]);
            if (!from_node || !to_node)
                continue;

            Pin *fp = from_node->outport_.FindPin(from_pin);
            Pin *tp = to_node->inport_.FindPin(to_pin);
            if (!fp || !tp)
                continue;

            dest.Connect(*fp, *tp);
        }
    }

    // --- Build widgets parallel to loaded modules ---
    ModelWidgetBuilder builder;
    std::vector<ModuleWidget *> loaded_widgets;
    for (Model *m : loaded_models) {
        auto *module = dynamic_cast<Module *>(m);
        if (!module) {
            loaded_widgets.push_back(nullptr);
            continue;
        }
        auto w = builder.Build(*module);
        auto *mw = dynamic_cast<ModuleWidget *>(w.get());
        if (mw)
            view.widget_map()[module->id_] = mw;
        view.root_->AddChild(std::move(w));
        loaded_widgets.push_back(mw);
    }

    // --- Load widget state ---
    if (view_j.contains("widgets") && view_j["widgets"].is_array()) {
        Archive view_archive(view_j);

        const auto &j_widgets = view_j["widgets"];
        size_t n = std::min(loaded_widgets.size(), j_widgets.size());
        for (size_t i = 0; i < n; ++i) {
            if (!loaded_widgets[i])
                continue;
            auto &j_w = const_cast<nlohmann::json &>(j_widgets[i]);
            JsonScope scope(view_archive, j_w);
            am.Deserialize(view_archive, *loaded_widgets[i]);

            loaded_widgets[i]->grid_position_ += paste_offset;
            loaded_widgets[i]->position_dirty_ = true;
        }
    }

    std::vector<Model *> result;
    for (Model *m : loaded_models) {
        if (dynamic_cast<Module *>(m))
            result.push_back(m);
    }
    return result;
}

bool RackSelection::LooksLikeSelection(const nlohmann::json &payload) {
    if (!payload.is_object())
        return false;

    auto fmt = payload.find("format");
    if (fmt == payload.end() || !fmt->is_string())
        return false;
    if (fmt->get<std::string>() != "augr.selection")
        return false;

    auto ver = payload.find("version");
    if (ver == payload.end() || !ver->is_number_integer())
        return false;
    if (ver->get<int>() != 1)
        return false;

    return true;
}

} // namespace augr