#include "../app/app.h"

#include "frame.h"

#include "imgui.h"

namespace augr {

bool Frame::is_active() {
    return App::singleton().active_frame() == this;
}

void Frame::Draw() {
    Begin();
    //DrawChildren();
    End();
}

void Frame::Begin() {
    ImGui::Begin(label_.c_str());
}

void Frame::End() {
    DrawChildren();
    ImGui::End();
}

} // namespace augr