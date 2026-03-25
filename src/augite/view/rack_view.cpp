#include "imgui.h"
#include "imgui_internal.h"

#include "imnodes.h"

#include <augr/core/rack/model_manufacturer.h>
#include <augr/core/rack/module/module.h>

#include <augr/core/rack/pin.h>
#include <augr/core/rack/rack.h>
#include <augr/core/rack/wire.h>

#include <augite/widget/module_widget.h>
#include <augite/widget/widget_builder.h>

#include "rack_view.h"

namespace augr {

RackView::RackView(Rack &rack) : ModelViewT<Rack>(rack) {}

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

    if (widget->model()) {
        widget_map_[widget->model()->id_] = widget;
    }

    // Recursively process children
    for (auto *child : widget->children_) {
        PopulateWidgetMap(child);
    }
}

void RackView::LoadEditorState() {
    ImNodes::LoadCurrentEditorStateFromIniFile("imnodes.ini");
}

void RackView::SaveEditorState() {
    ImNodes::SaveCurrentEditorStateToIniFile("imnodes.ini");
}

void RackView::Draw() {
    if (root_ == nullptr) {
        Build();
    }

    DrawMainDockspace();

    ImGuiWindowFlags graph_flags =
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoBringToFrontOnFocus; // <-- key

    ImGui::Begin("Graph", nullptr, graph_flags);

    ImNodes::BeginNodeEditor();

    root_->Draw();

    for (auto wire : model_->wires_) {
        ImNodes::Link(wire->id_, wire->output_->id_, wire->input_->id_);
    }

    is_editor_hovered_ = ImNodes::IsEditorHovered();

    ImNodes::EndNodeEditor();

    CheckMouse();

    CheckLinkCreated();
    CheckLinkDestroyed();
    CheckCreateNode();

    DrawModuleCatalog();

    // Optional: context menu contents
    if (ImGui::BeginPopup("node_ctx")) {
        ImGui::Text("Node %d", hovered_node_id);
        if (ImGui::MenuItem("Do thing")) { /* ... */
        }
        ImGui::EndPopup();
    }

    ImGui::End();
}

void RackView::CheckLinkCreated() {
    int startId, endId;
    bool createdFromSnap;
    if (ImNodes::IsLinkCreated(&startId, &endId, &createdFromSnap)) {
        Pin &output = *model_->output_map_[startId];
        Pin &input = *model_->input_map_[endId];
        model_->Connect(output, input);
    }
}

void RackView::CheckLinkDestroyed() {
    int linkId;
    if (ImNodes::IsLinkDestroyed(&linkId)) {
        auto wire = model_->wire_map_[linkId];
        model_->Disconnect(*wire);
    }
}

void RackView::CheckCreateNode() {
    int node_id = -1;

    if (ImNodes::IsLinkDropped(&pending_link_start_attr,
                               /*including_detach*/ false)) {
        pending_spawn_pos = ImGui::GetMousePos(); // screen space
        ImGui::OpenPopup("ModuleCatalog");
    }

    if (ImGui::IsMouseReleased(ImGuiMouseButton_Right) &&
        is_editor_hovered_ && !ImNodes::IsAnyAttributeActive() &&
        !ImGui::IsAnyItemHovered() && !ImNodes::IsNodeHovered(&node_id)) {
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

void RackView::DrawModuleCatalog() {
    if (ImGui::BeginPopup("ModuleCatalog")) {
        // Place popup under cursor
        ImGui::SetWindowPos(pending_spawn_pos, ImGuiCond_Always);

        // Simple filter + list (replace with your real catalog)
        static char filter[64] = "";
        ImGui::InputTextWithHint("##filter", "Search modules…", filter,
                                 IM_ARRAYSIZE(filter));

        for (const auto &it : ModelManufacturer::instance().factories_) {
            if (filter[0] && !strstr(it->name_.c_str(), filter))
                continue;

            if (ImGui::Selectable(it->name_.c_str())) {
                // Model& node = *it->Produce(*model_);
                Node &node = dynamic_cast<Node &>(*it->Produce(*model_));
                model_->AddChild(node);

                WidgetBuilder builder;
                Widget *widget = builder.Build(node);
                root_->AddChild(widget);
                widget_map_[node.id_] = widget;

                ImNodes::SetNodeScreenSpacePos(node.id_, pending_spawn_pos);

                // 5) If user was dragging a link, try to auto-connect
                if (pending_link_start_attr != -1) {
                    // Decide direction based on the starting pin’s kind
                    bool start_is_output = model_->IsOutputPin(
                        pending_link_start_attr); // your helper

                    Pin *output_pin = nullptr;
                    Pin *input_pin = nullptr;

                    if (start_is_output) {
                        // Find first input pin
                        output_pin =
                            model_->output_map_[pending_link_start_attr];
                        if (!node.inport_.pins_.empty()) {
                            input_pin = node.inport_.pins_[0];
                        }
                    } else {
                        // Find first output pin
                        if (!node.outport_.pins_.empty()) {
                            output_pin = node.outport_.pins_[0];
                        }
                        input_pin = model_->input_map_[pending_link_start_attr];
                    }
                    if (output_pin && input_pin) {
                        model_->Connect(*output_pin, *input_pin);
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

static bool s_built_dock = false;

void RackView::DrawMainDockspace() {
    ImGuiWindowFlags host_flags =
        ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_NoBackground; // optional, keeps it invisible

    ImGuiViewport *vp = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(vp->WorkPos);
    ImGui::SetNextWindowSize(vp->WorkSize);
    ImGui::SetNextWindowViewport(vp->ID);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

    ImGui::Begin("DockHost", nullptr, host_flags);

    ImGui::PopStyleVar(3);

    // Create a dockspace filling this host window
    ImGuiID dockspace_id = ImGui::GetID("MainDockspace");
    ImGuiDockNodeFlags dock_flags =
        ImGuiDockNodeFlags_PassthruCentralNode; // central node draws your
                                                // windows
    ImGui::DockSpace(dockspace_id, ImVec2(0, 0), dock_flags);

    // Build default layout once
    if (!s_built_dock) {
        s_built_dock = true;

        ImGui::DockBuilderRemoveNode(dockspace_id);
        ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspace_id, vp->WorkSize);

        ImGuiID dock_main_id = dockspace_id;
        ImGuiID dock_right, dock_bottom;
        dock_right = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right,
                                                 0.25f, nullptr, &dock_main_id);
        dock_bottom = ImGui::DockBuilderSplitNode(
            dock_main_id, ImGuiDir_Down, 0.30f, nullptr, &dock_main_id);

        ImGui::DockBuilderDockWindow("Graph", dock_main_id);   // central
        ImGui::DockBuilderDockWindow("Inspector", dock_right); // right
        ImGui::DockBuilderDockWindow("Console", dock_bottom);  // bottom
        ImGui::DockBuilderFinish(dockspace_id);
    }

    ImGui::End(); // DockHost
}

} // namespace augr