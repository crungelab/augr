#include "dock.h"
#include "imgui.h"

namespace augr {

void Dock::Draw() {
    Begin();
    //DrawChildren();
    End();
}

void Dock::Begin() {
    ImGui::Begin(label_.c_str());
}

void Dock::End() {
    DrawChildren();
    ImGui::End();
}

} // namespace augr