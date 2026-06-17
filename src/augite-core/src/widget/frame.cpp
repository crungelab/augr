#include <augite/app/app.h>

#include <augite/widget/frame.h>

#include "imgui.h"

namespace augr {

bool Frame::is_active() { return App::singleton().active_frame() == this; }

void Frame::Draw() {
    if (window_pose_dirty_ && !docked_) {
        ImGui::SetNextWindowPos(ToImVec2(window_position_));
        ImGui::SetNextWindowSize(ToImVec2(window_size_));
        window_pose_dirty_ = false;
    }

    if (is_active()) {
        DrawMainMenuBar();
    }

    Begin();
    docked_ = ImGui::IsWindowDocked();
    End();
}

void Frame::DrawMainMenuBar() {
    if (ImGui::BeginMainMenuBar()) {
        OnBeforeDrawMainMenuBar();
        OnDrawMainMenuBar();
    }
    ImGui::EndMainMenuBar();
}

void Frame::OnBeforeDrawMainMenuBar() {
    Frame *parent_viewer = dynamic_cast<Frame *>(parent_);
    if (parent_viewer == nullptr)
        return;
    parent_viewer->OnDrawMainMenuBar();
    parent_viewer->OnBeforeDrawMainMenuBar();
}

void Frame::OnDrawMainMenuBar() {
}

void Frame::Begin() { ImGui::Begin(label_.c_str()); }

void Frame::End() {
    DrawChildren();
    if (!docked_) {
        window_position_ = FromImVec2(ImGui::GetWindowPos());
        window_size_ = FromImVec2(ImGui::GetWindowSize());
    }
    ImGui::End();
}

} // namespace augr