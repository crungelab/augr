// rack_controller.cpp
#include "rack_controller.h"

#include <iostream>

#include <imgui.h>
#include <imnodes.h>
#include <nlohmann/json.hpp>

#include <augr/core/model_manufacturer.h>
#include <augr/rack/module/module.h>
#include <augr/rack/pin.h>
#include <augr/rack/rack.h>
#include <augr/rack/rack_doc.h>
#include <augr/rack/wire.h>

#include <augite/widget/module_widget.h>
#include <augite/widget/widget_builder.h>

#include "rack_selection.h"

namespace augr {

RackController::RackController(RackDoc &doc, RackView &view)
    : ControllerT<RackDoc, RackView>(doc, view) {}

Rack &RackController::rack() { return doc().rack(); }
const Rack &RackController::rack() const { return doc().rack(); }

// ----- Per-frame entry point -----

void RackController::Control() {
    CheckPendingSelection();
    CheckMouse();
    CheckLinkCreated();
    CheckLinkDestroyed();
    CheckCreateNode();
    CheckNodeSelection();
    CheckClipboard();
    DrawCatalogPopup();
    DrawNodeContextMenu();
}

// ----- Selection -----

void RackController::CollectSelection(
    std::vector<Model *> &out_models,
    std::vector<Widget *> &out_widgets) const {
    out_models.clear();
    out_widgets.clear();
    const auto &wmap = view().widget_map();
    for (int id : selected_nodes_) {
        auto it = wmap.find(id);
        if (it == wmap.end())
            continue;
        auto *mw = dynamic_cast<ModelWidget *>(it->second);
        if (!mw)
            continue;
        Model *m = mw->model();
        if (!m)
            continue;
        out_models.push_back(m);
        out_widgets.push_back(mw);
    }
}

// ----- Actions -----

void RackController::Copy() {
    std::vector<Model *> models;
    std::vector<Widget *> widgets;
    CollectSelection(models, widgets);
    if (models.empty())
        return;

    nlohmann::json j =
        RackSelection::BuildSelectionJson(rack(), view(), models, widgets);
    ImGui::SetClipboardText(j.dump().c_str());
}

void RackController::Paste() {
    const char *text = ImGui::GetClipboardText();
    if (!text)
        return;

    try {
        nlohmann::json j = nlohmann::json::parse(text);
        if (!RackSelection::LooksLikeSelection(j))
            return;

        // Fixed offset for now; cursor-anchored paste is a later enhancement.
        Vec2 offset = {20.0f, 20.0f};
        auto pasted = RackSelection::MergeIntoRack(rack(), view(), j, offset);

        SetPendingSelection(pasted);
        doc().MarkModified();
    } catch (const std::exception &e) {
        std::cerr << "Paste failed: " << e.what() << "\n";
    }
}

void RackController::Cut() {
    if (!HasSelection())
        return;
    Copy();
    DeleteSelection();
}

void RackController::Duplicate() {
    if (!HasSelection())
        return;

    std::vector<Model *> models;
    std::vector<Widget *> widgets;
    CollectSelection(models, widgets);

    nlohmann::json j =
        RackSelection::BuildSelectionJson(rack(), view(), models, widgets);

    Vec2 offset = {20.0f, 20.0f};
    auto pasted = RackSelection::MergeIntoRack(rack(), view(), j, offset);

    SetPendingSelection(pasted);
    doc().MarkModified();
}

void RackController::DeleteSelection() {
    if (!HasSelection())
        return;

    auto &wmap = view().widget_map();
    Widget *root = view().root_;

    for (int node_id : selected_nodes_) {
        auto it = wmap.find(node_id);
        if (it == wmap.end())
            continue;
        Widget *widget = it->second;
        auto *mw = dynamic_cast<ModelWidget *>(widget);
        if (!mw)
            continue;

        if (auto *mod = dynamic_cast<Module *>(mw->model())) {
            rack().RemoveChild(*mod);
        }
        if (root)
            root->RemoveChild(*widget);
        wmap.erase(it);
        delete widget;
    }
    selected_nodes_.clear();
    ImNodes::ClearNodeSelection();
    doc().MarkModified();
}

Module *RackController::SpawnModule(const std::string &type_name, Vec2 grid_pos,
                                    int auto_connect_pin_id) {
    auto &mm = ModelManufacturer::singleton();
    ModelFactory *mf = mm.FindFactory(type_name);
    if (!mf) {
        std::cerr << "SpawnModule: unknown type '" << type_name << "'\n";
        return nullptr;
    }

    Module &module = dynamic_cast<Module &>(*mf->Produce(&rack()));

    ModelWidgetBuilder builder;
    Widget *widget = builder.Build(module);
    if (view().root_) {
        view().root_->AddChild(widget);
    }
    if (auto *mw = dynamic_cast<ModuleWidget *>(widget)) {
        view().widget_map()[module.id_] = mw;
        mw->grid_position_ = grid_pos;
        mw->position_dirty_ = true;
    }

    // Auto-connect from a dropped link, if requested.
    if (auto_connect_pin_id != -1) {
        bool start_is_output = rack().IsOutput(auto_connect_pin_id);
        Pin *output = nullptr;
        Pin *input = nullptr;
        if (start_is_output) {
            output = rack().output_map_[auto_connect_pin_id];
            if (!module.inport_.pins_.empty()) {
                input = module.inport_.pins_[0];
            }
        } else {
            if (!module.outport_.pins_.empty()) {
                output = module.outport_.pins_[0];
            }
            input = rack().input_map_[auto_connect_pin_id];
        }
        if (output && input) {
            rack().Connect(*output, *input);
        }
    }

    doc().MarkModified();
    return &module;
}

// ----- Input polling -----

void RackController::CheckMouse() {
    int node_id = -1;
    if (ImNodes::IsNodeHovered(&node_id)) {
        hovered_node_id_ = node_id;

        if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
            const auto &wmap = view().widget_map();
            if (auto it = wmap.find(hovered_node_id_); it != wmap.end()) {
                if (auto *mw = dynamic_cast<ModuleWidget *>(it->second)) {
                    mw->is_open_ = !mw->is_open_;
                    mw->window_pose_dirty_ = true;
                }
            }
        }

        if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
            ImGui::OpenPopup("node_ctx");
        }
    }
}

void RackController::CheckLinkCreated() {
    int start_id, end_id;
    bool from_snap;
    if (ImNodes::IsLinkCreated(&start_id, &end_id, &from_snap)) {
        Pin *out = rack().output_map_[start_id];
        Pin *in = rack().input_map_[end_id];
        if (out && in) {
            rack().Connect(*out, *in);
            doc().MarkModified();
        }
    }
}

void RackController::CheckLinkDestroyed() {
    int link_id;
    if (ImNodes::IsLinkDestroyed(&link_id)) {
        auto it = rack().wire_map_.find(link_id);
        if (it != rack().wire_map_.end()) {
            rack().Disconnect(*it->second);
            doc().MarkModified();
        }
    }
}

void RackController::CheckCreateNode() {
    int dummy = -1;

    if (ImNodes::IsLinkDropped(&pending_link_start_attr_,
                               /*including_detach=*/false)) {
        pending_spawn_pos_ = ImGui::GetMousePos();
        catalog_popup_requested_ = true;
    }

    if (ImGui::IsMouseReleased(ImGuiMouseButton_Right) &&
        view().is_editor_hovered() && !ImNodes::IsAnyAttributeActive() &&
        !ImGui::IsAnyItemHovered() && !ImNodes::IsNodeHovered(&dummy)) {
        pending_link_start_attr_ = -1;
        pending_spawn_pos_ = ImGui::GetMousePos();
        catalog_popup_requested_ = true;
    }
}

void RackController::CheckNodeSelection() {
    const int num = ImNodes::NumSelectedNodes();
    if (num > 0) {
        selected_nodes_.resize(num);
        ImNodes::GetSelectedNodes(selected_nodes_.data());
    } else {
        selected_nodes_.clear();
    }

    if (ImGui::IsKeyPressed(ImGuiKey_Escape, /*repeat=*/false)) {
        ImNodes::ClearNodeSelection();
        selected_nodes_.clear();
    } else if (ImGui::IsKeyPressed(ImGuiKey_Delete, /*repeat=*/false)) {
        DeleteSelection();
    }
}

void RackController::CheckClipboard() {
    if (!view().is_editor_hovered())
        return;
    if (ImGui::GetIO().WantTextInput)
        return;

    const bool ctrl = ImGui::GetIO().KeyCtrl;
    if (!ctrl)
        return;

    if (ImGui::IsKeyPressed(ImGuiKey_C, /*repeat=*/false)) {
        Copy();
    } else if (ImGui::IsKeyPressed(ImGuiKey_V, /*repeat=*/false)) {
        Paste();
    } else if (ImGui::IsKeyPressed(ImGuiKey_X, /*repeat=*/false)) {
        Cut();
    } else if (ImGui::IsKeyPressed(ImGuiKey_D, /*repeat=*/false)) {
        Duplicate();
    }
}

// ----- Popups (controller-owned) -----

void RackController::DrawCatalogPopup() {
    if (catalog_popup_requested_) {
        ImGui::OpenPopup("ModuleCatalog");
        catalog_popup_requested_ = false;
    }

    if (ImGui::BeginPopup("ModuleCatalog")) {
        ImGui::SetWindowPos(pending_spawn_pos_, ImGuiCond_Appearing);

        static char filter[64] = "";
        ImGui::InputTextWithHint("##filter", "Search modules…", filter,
                                 IM_ARRAYSIZE(filter));

        auto &mm = ModelManufacturer::singleton();
        for (const auto &f : mm.factories_) {
            if (f->category_.empty())
                continue;
            if (filter[0] && !strstr(f->name_.c_str(), filter))
                continue;

            if (ImGui::Selectable(f->name_.c_str())) {
                // Convert screen → grid by going through ImNodes once.
                // (Same trick as the catalog spawn before this refactor.)
                ImVec2 dummy_screen = pending_spawn_pos_;
                Module *m =
                    SpawnModule(f->name_, Vec2{0, 0}, pending_link_start_attr_);
                if (m) {
                    // Re-position via ImNodes' screen→grid round-trip.
                    ImNodes::SetNodeScreenSpacePos(m->id_, dummy_screen);
                    ImVec2 grid = ImNodes::GetNodeGridSpacePos(m->id_);
                    auto &wmap = view().widget_map();
                    if (auto it = wmap.find(m->id_); it != wmap.end()) {
                        if (auto *mw =
                                dynamic_cast<ModuleWidget *>(it->second)) {
                            mw->grid_position_ = ModuleWidget::FromImVec2(grid);
                            mw->position_dirty_ = false;
                        }
                    }
                }

                pending_link_start_attr_ = -1;
                ImGui::CloseCurrentPopup();
            }
        }

        ImGui::EndPopup();
    }
}

void RackController::DrawNodeContextMenu() {
    if (ImGui::BeginPopup("node_ctx")) {
        ImGui::Text("Node %d", hovered_node_id_);
        if (ImGui::MenuItem("Do thing")) {
            // ...
        }
        ImGui::EndPopup();
    }
}

// ----- Helpers -----

void RackController::SetPendingSelection(const std::vector<Model *> &models) {
    pending_selection_.clear();
    for (Model *m : models) {
        if (m)
            pending_selection_.push_back(m->id_);
    }
}

void RackController::CheckPendingSelection() {
    if (pending_selection_.empty())
        return;

    ImNodes::ClearNodeSelection();
    for (int id : pending_selection_) {
        ImNodes::SelectNode(id);
    }
    selected_nodes_ = pending_selection_;
    pending_selection_.clear();
}

} // namespace augr