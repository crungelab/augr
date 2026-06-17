#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h> // Required for std::string overloads

#include <augr/rack/module/module.h>

#include <augite/inspector/module_inspector.h>

namespace augr {

void ModuleInspector::Draw() {
    //ImGui::TextUnformatted("Module inspector content goes here.");
    ImGui::InputText("Label", &model().label_);
}
DEFINE_MODEL_WIDGET_FACTORY(ModuleInspector, Module)

} // namespace augr