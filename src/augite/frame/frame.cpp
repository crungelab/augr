#include "frame.h"

#include "imgui.h"

namespace augr {

void Frame::Draw() {
    ImGui::Begin(label_.c_str());
    if (view_) {
        view_->Draw();
    }
    DrawChildren();
    ImGui::End();
}

} // namespace augr