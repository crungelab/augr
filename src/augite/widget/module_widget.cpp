#include "imgui.h"
#include "imnodes.h"

#include <augr/core/rack/module/module.h>

#include "module_widget.h"

namespace augr {

void ModuleWidget::Draw() {
    DrawNode();

    if (is_open_) {
        DrawWindow();
    }
}

void ModuleWidget::DrawWindow() {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
    ImGui::Begin(model_->label_, &is_open_);
    
    DrawChildren();

    ImGui::End();
    ImGui::PopStyleVar();
}

void ModuleWidget::DrawNode() {
    ImNodes::BeginNode(model_->id_);

    ImNodes::BeginNodeTitleBar();
    ImGui::TextUnformatted(model_->label_);
    ImNodes::EndNodeTitleBar();

    for (auto input : model_->inport_.pins_) {
        ImNodes::BeginInputAttribute(input->id_);
        ImGui::TextUnformatted(input->name_.c_str());
        ImNodes::EndInputAttribute();
    }

    for (auto output : model_->outport_.pins_) {
        ImNodes::BeginOutputAttribute(output->id_);
        ImGui::Indent(40);
        ImGui::TextUnformatted(output->name_.c_str());
        ImNodes::EndOutputAttribute();
    }

    ImNodes::EndNode();
}

DEFINE_WIDGET_FACTORY(ModuleWidget, Module)

} // namespace augr