#include "dock.h"
#include "imgui.h"

namespace augr {

void Dock::Draw() {
    ImGui::Begin("Dock");
    DrawChildren();
    ImGui::End();
}

void Dock::Begin() {
    ImGui::Begin(label_.c_str());
}

void Dock::End() {
    ImGui::End();
}

} // namespace augr