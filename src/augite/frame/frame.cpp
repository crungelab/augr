#include "../app/app.h"

#include "frame.h"

#include "imgui.h"

namespace augr {

void Frame::End() {
    if (view_) view_->Draw();
    if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)) {
        App::singleton().set_active_frame(this);
        if (controller_) controller_->Control();
    }
    //if (controller_) controller_->Control();

    Dock::End();
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