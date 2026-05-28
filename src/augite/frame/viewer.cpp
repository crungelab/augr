#include "../app/app.h"

#include "viewer.h"

#include "imgui.h"

namespace augr {

void Viewer::End() {
    if (view_) view_->Draw();
    if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)) {
        App::singleton().set_active_frame(this);
        if (controller_) controller_->Control();
    }

    Frame::End();
}

/*
void Frame::Draw() {
    ImGui::Begin(label_.c_str());

    if (view_) view_->Draw();
    if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)) {
        App::singleton().set_active_frame(this);
        if (controller_) controller_->Control();
    }
    //if (controller_) controller_->Control();

    DrawChildren();
    ImGui::End();
}
*/

} // namespace augr