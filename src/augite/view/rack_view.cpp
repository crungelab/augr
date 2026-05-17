#include <spdlog/spdlog.h>

#include "imgui.h"
#include "imgui_internal.h"

#include "imnodes.h"

#include <augr/core/model_manufacturer.h>

#include <augr/rack/module/module.h>

#include <augr/rack/pin.h>
#include <augr/rack/wire.h>
#include <augr/rack/rack.h>
#include <augr/rack/rack_doc.h>

#include <augite/widget/module_widget.h>
#include <augite/widget/widget_builder.h>

#include "rack_view.h"
#include "rack_selection.h"

namespace augr {

RackView::RackView(RackDoc &doc) : DocumentViewT<RackDoc>(doc), rack_(&doc.rack()) {}

RackView::~RackView() {
    if (root_) {
        delete root_;
        root_ = nullptr;
    }
}

void RackView::Build() {
    ModelView::Build(); // Call parent Build()

    // After build completes, traverse root_ and populate widget_map_
    if (root_) {
        PopulateWidgetMap(root_);
    }
}

// Helper to recursively populate widget_map_
void RackView::PopulateWidgetMap(Widget *widget) {
    if (!widget)
        return;

    if (auto *mw = dynamic_cast<ModelWidget *>(widget)) {
        widget_map_[mw->model()->id_] = mw;
    }

    for (auto *child : widget->children_) {
        PopulateWidgetMap(child);
    }
}

void RackView::Draw() {
    if (root_ == nullptr) {
        Build();
    }

    ImNodes::BeginNodeEditor();

    root_->Draw();

    for (auto wire : rack_->wires_) {
        ImNodes::Link(wire->id_, wire->output_->id_, wire->input_->id_);
    }

    is_editor_hovered_ = ImNodes::IsEditorHovered();

    ImNodes::EndNodeEditor();

    CheckMouse();

    CheckLinkCreated();
    CheckLinkDestroyed();
    CheckCreateNode();
    CheckNodeSelection();
    CheckClipboard();

    DrawModuleCatalog();

    // Optional: context menu contents
    if (ImGui::BeginPopup("node_ctx")) {
        ImGui::Text("Node %d", hovered_node_id);
        if (ImGui::MenuItem("Do thing")) { /* ... */
        }
        ImGui::EndPopup();
    }
    selected_nodes_.clear();
}

std::vector<Model*> RackView::SelectedModules() const {
    std::vector<Model*> result;
    for (int id : selected_nodes_) {
        auto it = widget_map_.find(id);
        if (it == widget_map_.end()) continue;
        if (auto* mw = dynamic_cast<ModelWidget*>(it->second)) {
            if (auto* m = dynamic_cast<Module*>(mw->model())) {
                result.push_back(m);
            }
        }
    }
    return result;
}

std::vector<Widget*> RackView::SelectedWidgets() const {
    std::vector<Widget*> result;
    for (int id : selected_nodes_) {
        auto it = widget_map_.find(id);
        if (it == widget_map_.end()) continue;
        if (auto* mw = dynamic_cast<ModuleWidget*>(it->second)) {
            result.push_back(mw);
        }
    }
    return result;
}

void RackView::CheckClipboard() {
    // Only act on shortcuts when the editor (not some other ImGui widget)
    // has focus. is_editor_hovered_ is a reasonable proxy.
    if (!is_editor_hovered_) return;

    // Don't fire shortcuts while text input is active (e.g. catalog
    // search field).
    if (ImGui::GetIO().WantTextInput) return;

    const bool ctrl = ImGui::GetIO().KeyCtrl;
    if (!ctrl) return;

    if (ImGui::IsKeyPressed(ImGuiKey_C, /*repeat=*/false)) {
        HandleCopy();
    } else if (ImGui::IsKeyPressed(ImGuiKey_V, /*repeat=*/false)) {
        HandlePaste();
    } else if (ImGui::IsKeyPressed(ImGuiKey_X, /*repeat=*/false)) {
        HandleCut();   // copy + delete selection
    } else if (ImGui::IsKeyPressed(ImGuiKey_D, /*repeat=*/false)) {
        HandleDuplicate();  // copy + paste in one step with auto-offset
    }
}

void RackView::HandleCopy() {
    auto modules = SelectedModules();
    auto widgets = SelectedWidgets();
    if (modules.empty()) return;

    RackSelection selection;
    nlohmann::json j = selection.BuildSelectionJson(*rack_, *this, modules, widgets);
    ImGui::SetClipboardText(j.dump().c_str());
}

void RackView::HandlePaste() {
    const char* text = ImGui::GetClipboardText();
    if (!text) return;
    try {
        nlohmann::json j = nlohmann::json::parse(text);
        // Compute offset — for now, just a fixed nudge.
        Vec2 offset = {20, 20};
        RackSelection selection;
        selection.MergeSelectionIntoRack(*rack_, *this, j, offset);
    } catch (...) {
        // Clipboard didn't contain valid selection JSON. Ignore.
    }
}

void RackView::HandleCut() {
    HandleCopy();
}

void RackView::HandleDuplicate() {
    auto modules = SelectedModules();
    auto widgets = SelectedWidgets();
    if (modules.empty()) return;

    RackSelection selection;
    nlohmann::json j = selection.BuildSelectionJson(*rack_, *this, modules, widgets);
    // Compute offset — for now, just a fixed nudge.
    Vec2 offset = {20, 20};
    selection.MergeSelectionIntoRack(*rack_, *this, j, offset);
}

void RackView::CheckLinkCreated() {
    int startId, endId;
    bool createdFromSnap;
    if (ImNodes::IsLinkCreated(&startId, &endId, &createdFromSnap)) {
        Pin &output = *rack_->output_map_[startId];
        Pin &input = *rack_->input_map_[endId];
        rack_->Connect(output, input);
    }
}

void RackView::CheckLinkDestroyed() {
    int linkId;
    if (ImNodes::IsLinkDestroyed(&linkId)) {
        auto wire = rack_->wire_map_[linkId];
        spdlog::debug("Link destroyed: {}", linkId);
        rack_->Disconnect(*wire);
    }
}

void RackView::CheckCreateNode() {
    int node_id = -1;

    if (ImNodes::IsLinkDropped(&pending_link_start_attr,
                               /*including_detach*/ false)) {
        pending_spawn_pos = ImGui::GetMousePos(); // screen space
        ImGui::OpenPopup("ModuleCatalog");
    }

    if (ImGui::IsMouseReleased(ImGuiMouseButton_Right) && is_editor_hovered_ &&
        !ImNodes::IsAnyAttributeActive() && !ImGui::IsAnyItemHovered() &&
        !ImNodes::IsNodeHovered(&node_id)) {
        pending_link_start_attr = -1; // no pending link
        pending_spawn_pos = ImGui::GetMousePos();
        ImGui::OpenPopup("ModuleCatalog");
    }
}

void RackView::CheckMouse() {
    int node_id = -1;
    if (ImNodes::IsNodeHovered(&node_id)) {
        hovered_node_id = node_id;
        // Left double-click on a hovered node
        if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
            // handle double-click on hovered_node_id
            if (const auto it = widget_map_.find(hovered_node_id);
                it != widget_map_.end()) {
                Widget *widget = it->second;
                if (auto *module_widget =
                        dynamic_cast<ModuleWidget *>(widget)) {
                    module_widget->is_open_ = !module_widget->is_open_;
                }
            }
        }

        // Right click on a hovered node
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
            // handle right-click on hovered_node_id
            // e.g. open a context menu:
            ImGui::OpenPopup("node_ctx");
        }
    }
}

void RackView::CheckNodeSelection() {
    const int num_selected_nodes = ImNodes::NumSelectedNodes();
    if (num_selected_nodes > 0) {
        selected_nodes_.resize(num_selected_nodes);
        ImNodes::GetSelectedNodes(selected_nodes_.data());
    }
    if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
        ImNodes::ClearNodeSelection();
        //selected_nodes_.clear();
    } else if (ImGui::IsKeyPressed(ImGuiKey_Delete)) {
        for (int node_id : selected_nodes_) {
            if (const auto it = widget_map_.find(node_id);
                it != widget_map_.end()) {
                Widget *widget = it->second;
                if (auto *module_widget =
                        dynamic_cast<ModuleWidget *>(widget)) {
                    rack_->RemoveChild(*module_widget->model());
                    root_->RemoveChild(*widget);
                    widget_map_.erase(it);
                    delete widget;
                }
            }
        }
        //selected_nodes_.clear();
    }
}

void RackView::DrawModuleCatalog() {
    if (ImGui::BeginPopup("ModuleCatalog")) {
        // Place popup under cursor
        ImGui::SetWindowPos(pending_spawn_pos, ImGuiCond_Always);

        // Simple filter + list (replace with your real catalog)
        static char filter[64] = "";
        ImGui::InputTextWithHint("##filter", "Search modules…", filter,
                                 IM_ARRAYSIZE(filter));

        for (const auto &it : ModelManufacturer::singleton().factories_) {
            if (it->category_ == "")
                continue; // don't allow spawning models without category
            if (filter[0] && !strstr(it->name_.c_str(), filter))
                continue;

            if (ImGui::Selectable(it->name_.c_str())) {
                Module &module = dynamic_cast<Module &>(*it->Produce(rack_));

                ModelWidgetBuilder builder;
                Widget *widget = builder.Build(module);
                root_->AddChild(widget);
                widget_map_[module.id_] = widget;

                ImNodes::SetNodeScreenSpacePos(module.id_, pending_spawn_pos);

                // 5) If user was dragging a link, try to auto-connect
                if (pending_link_start_attr != -1) {
                    // Decide direction based on the starting pin’s kind
                    bool start_is_output = rack_->IsOutput(
                        pending_link_start_attr); // your helper

                    Pin *output = nullptr;
                    Pin *input = nullptr;

                    if (start_is_output) {
                        // Find first input
                        output =
                            rack_->output_map_[pending_link_start_attr];
                        if (!module.inport_.pins_.empty()) {
                            input = module.inport_.pins_[0];
                        }
                    } else {
                        // Find first output
                        if (!module.outport_.pins_.empty()) {
                            output = module.outport_.pins_[0];
                        }
                        input = rack_->input_map_[pending_link_start_attr];
                    }
                    if (output && input) {
                        rack_->Connect(*output, *input);
                    }
                    // pending_link_start_attr = -1;
                }

                pending_link_start_attr = -1;
                ImGui::CloseCurrentPopup();
            }
        }

        ImGui::EndPopup();
    }
}

} // namespace augr