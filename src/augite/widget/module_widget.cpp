// #include "imgui.h"
// #include "imnodes.h"

#include <augr/rack/module/module.h>

#include "module_widget.h"

namespace augr {

void ModuleWidget::DrawInspector() {
    if (ImGui::Begin("Inspector", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
    //if (ImGui::Begin("Inspector")) {
        DrawInspectorContent();
    }

    ImGui::End();
}

void ModuleWidget::DrawInspectorContent() {
    ImGui::TextUnformatted(model().label_.c_str());
}

DEFINE_MODEL_WIDGET_FACTORY(DefaultModuleWidget, Module)

} // namespace augr