#include "imgui.h"
#include "imgui_internal.h"

#include <augr/rack/rack.h>
#include <augr/rack/rack_doc.h>
#include "../frame/rack_frame.h"

#include "rack_app.h"

namespace augr {

RackApp *RackApp::singleton_;

RackApp::RackApp() {
    singleton_ = this;
    doc_ = std::make_unique<RackDoc>();
    static_cast<RackDoc *>(doc_.get())->NewDocument();
    root_frame_ = std::make_unique<RackFrame>(*static_cast<RackDoc *>(doc_.get()), *static_cast<Rack *>(doc_->model()), "Rack");
    root_frame_->Create();
}

void RackApp::Draw() {
    DrawMainDockspace();
    App::Draw();
}

static bool s_built_dock = false;

void RackApp::DrawMainDockspace() {
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

        ImGui::DockBuilderDockWindow("Rack", dock_main_id);   // central
        ImGui::DockBuilderDockWindow("Inspector", dock_right); // right
        ImGui::DockBuilderDockWindow("Console", dock_bottom);  // bottom
        ImGui::DockBuilderFinish(dockspace_id);
    }

    ImGui::End(); // DockHost
}


} // namespace augr