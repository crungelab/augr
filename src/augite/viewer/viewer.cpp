#include "../app/app.h"

#include "viewer.h"

#include "imgui.h"

namespace augr {

void Viewer::Begin() {
    // Use a unique ImGui window ID per viewer so multiple frames
    // (different viewers) don't collide. The viewer pointer is stable
    // for the frame's lifetime.
    char title[128];
    std::snprintf(title, sizeof(title), "%s###viewer_%p",
                  label_.empty() ? "Viewer" : label_.c_str(),
                  static_cast<void *>(&model()));
    bool p_open = true;
    ImGui::Begin(title, &p_open, ImGuiWindowFlags_NoCollapse);
    if (!p_open) {
        App::singleton().viewer_manager().CloseViewer(*this);
    }
}

void Viewer::End() {
    if (view_) view_->Draw();
    if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)) {
        App::singleton().set_active_frame(this);
        if (controller_) controller_->Control();
    }

    Frame::End();
}

} // namespace augr