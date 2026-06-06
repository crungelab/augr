#include <imgui.h>

#include "inspector_dock.h"

namespace augr {

void InspectorDock::Draw() {
    if (ImGui::Begin("Inspector", nullptr, ImGuiWindowFlags_NoFocusOnAppearing)) {
        root_->Draw();
    }

    ImGui::End();
}
} // namespace augr