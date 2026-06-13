#include "imgui.h"
#include "imgui_internal.h"

#include "../viewer/rack_viewer.h"
#include <augr/rack/rack.h>
#include <augr/rack/rack_doc.h>

#include "rack_app.h"

namespace augr {

RackApp *RackApp::singleton_;

RackApp::RackApp() {
    singleton_ = this;
    doc_ = std::make_unique<RackDoc>();
    on_doc_unload_conn_ = doc_->on_unload.connect([this]() {
        Inspect(nullptr);
        if (root_frame_) {
            root_frame_->DestroyChildren();
        }
    });
    document().NewDocument();
}

bool RackApp::DoCreate(CreateParams params) {
    bool success = FrameApp::DoCreate(params);

    root_frame_ =
        std::make_unique<RackViewer>("Rack", document(), document().model());
    root_frame_->Create();
    set_active_frame(root_frame_.get());

    return success;
}

void RackApp::Draw() {
    DrawMainDockspace();
    App::Draw();
}

void RackApp::DrawMainDockspace() {
    ImGuiID dockspace_id = ImGui::GetID("MainDockspace");
    ImGuiViewport *vp = ImGui::GetMainViewport();

    // Build default layout once
    if (ImGui::DockBuilderGetNode(dockspace_id) == nullptr) {
        ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspace_id, vp->WorkSize);

        ImGuiID dock_main_id = dockspace_id;
        ImGuiID dock_right, dock_bottom;
        ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.25f,
                                    &dock_right, &dock_main_id);
        ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.30f,
                                    &dock_bottom, &dock_main_id);

        ImGui::DockBuilderDockWindow("Rack", dock_main_id);
        ImGui::DockBuilderDockWindow("Inspector", dock_right);
        ImGui::DockBuilderDockWindow("Console", dock_bottom);
        ImGui::DockBuilderFinish(dockspace_id);
    }

    // Submit dockspace
    ImGui::DockSpaceOverViewport(dockspace_id, vp,
                                 ImGuiDockNodeFlags_PassthruCentralNode);
}

} // namespace augr