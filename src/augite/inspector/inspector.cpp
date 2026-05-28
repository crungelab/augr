#include <imgui.h>

#include "inspector.h"

namespace augr {

void Inspector::Draw() {
    if (ImGui::Begin("Inspector", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::TextUnformatted("Inspector content goes here.");
    }

    ImGui::End();
}
} // namespace augr