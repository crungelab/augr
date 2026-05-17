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

    DrawModuleCatalog();

    // Optional: context menu contents
    if (ImGui::BeginPopup("node_ctx")) {
        ImGui::Text("Node %d", hovered_node_id);
        if (ImGui::MenuItem("Do thing")) { /* ... */
        }
        ImGui::EndPopup();
    }
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
    // This is a placeholder for node selection logic, which can be implemented
    // based on your specific requirements. You can use ImNodes functions to
    // check for node selection and update the selected_nodes_ vector
    // accordingly.
    const int num_selected_nodes = ImNodes::NumSelectedNodes();
    if (num_selected_nodes > 0) {
        selected_nodes_.resize(num_selected_nodes);
        ImNodes::GetSelectedNodes(selected_nodes_.data());
    }
    if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
        ImNodes::ClearNodeSelection();
        selected_nodes_.clear();
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
        selected_nodes_.clear();
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