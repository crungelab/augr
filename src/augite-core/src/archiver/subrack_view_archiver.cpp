#include <iostream>

#include <augite/archiver/subrack_view_archiver.h>

#include <augr/archive.h>
#include <augr/archiver_factory.h>
#include <augr/archiver_manufacturer.h>

#include <augite/widget/module_widget.h>
#include <augite/widget/widget_builder.h>

#include <nlohmann/json.hpp>

namespace augr {

void SubrackViewArchiver::Save(Archive &archive) const {
    Archiver::Save(archive);

    const SubrackView &view = subject();
    auto &j = archive.json();

    // View-level state (pan, zoom, etc.) goes here in the future.
    // For now, just the widgets array.

    SaveWidgets(archive, view.child_pointers());
}

void SubrackViewArchiver::SaveWidgets(
    Archive &archive, const std::vector<Widget *> &widgets) const {
    const SubrackView &view = subject();

    auto &j = archive.json();

    auto &j_widgets = j["widgets"] = nlohmann::json::array();

    for (Widget *child : widgets) {
        if (auto *module_widget = dynamic_cast<ModuleWidget *>(child)) {
            if (module_widget->model().is_replicated()) {
                // Don't save widgets for replicated modules
                continue;
            }
        }
        nlohmann::json j_widget = nlohmann::json::object();
        {
            JsonScope scope(archive, j_widget);
            ArchiverManufacturer::singleton().Serialize(archive, *child);
        }

        // Temporary: catch which widget produces bad UTF-8
        try {
            j_widget.dump();
        } catch (const std::exception &e) {
            std::cerr << "Bad UTF-8 in widget archiver for: "
                      << typeid(*child).name() << " : " << e.what() << "\n";
        }

        j_widgets.push_back(std::move(j_widget));
    }
}

void SubrackViewArchiver::Load(Archive &archive) { LoadWidgets(archive); }

void SubrackViewArchiver::LoadWidgets(Archive &archive) {
    const auto &j = archive.json();
    SubrackView &view = subject();

    if (!j.contains("widgets") || !j["widgets"].is_array())
        return;
    const auto &j_widgets = j["widgets"];

    // Sanity check: widget count must match. If not, log and load what
    // we can.

    const auto &children = view.children_;
    const size_t n = std::min(children.size(), j_widgets.size());
    if (children.size() != j_widgets.size()) {
        // Could mean: rack loaded different children than the view JSON
        // expected (file from an older format, or modules failed to load).
        // Load the overlap; warn.
        std::cerr << "SubrackView load: widget count mismatch (have "
                  << children.size() << ", json has " << j_widgets.size()
                  << "). Loading first " << n << ".\n";
    }

    auto &mfr = ArchiverManufacturer::singleton();
    for (size_t i = 0; i < n; ++i) {
        const auto &j_widget = j_widgets[i];
        if (!j_widget.contains("type"))
            continue;

        // Optional: sanity-check the type tag matches what the widget
        // actually is. For now, just deserialize into whatever's there.
        JsonScope scope(archive, const_cast<nlohmann::json &>(j_widget));
        mfr.Deserialize(archive, *children[i]);
    }
}

DEFINE_ARCHIVER_FACTORY(SubrackViewArchiver, SubrackView, "SubrackView")

} // namespace augr
