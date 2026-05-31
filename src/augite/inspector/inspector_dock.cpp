#include <imgui.h>

#include "inspector_dock.h"

namespace augr {

void InspectorDock::Draw() {
    if (ImGui::Begin("Inspector", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        //ImGui::TextUnformatted("Inspector content goes here.");
        root_->Draw();
    }

    ImGui::End();
}
} // namespace augr