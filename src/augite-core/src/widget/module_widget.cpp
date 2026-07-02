// #include "imgui.h"
// #include "imnodes.h"

#include <augr/rack/module/module.h>

#include <augite/widget/module_widget.h>

namespace augr {

void ModuleWidget::Draw() {
    // Push persisted position on the first draw after load/spawn.
    if (this->position_dirty_) {
        ImNodes::SetNodeGridSpacePos(
            this->model().id_, ModuleWidget::ToImVec2(this->grid_position_));
        this->position_dirty_ = false;
    }

    ImNodes::BeginNode(this->model().id_);

    bool muted = model().muted_;

    ImNodes::BeginNodeTitleBar();
    if (ImGui::RadioButton("##mute", muted)) {
        model().muted_ = !muted;
    }
    ImGui::SameLine();
    ImGui::TextUnformatted(this->model().label_.c_str());
    ImNodes::EndNodeTitleBar();

    if (!this->model().inport_.pins_.empty()) {
        ImGui::BeginGroup();
        for (auto input : this->model().inport_.pins_) {
            ImNodes::BeginInputAttribute(input->id_);
            ImGui::TextUnformatted(input->name_.c_str());
            ImNodes::EndInputAttribute();
        }
        ImGui::EndGroup();
    } else {
        ImGui::Dummy(ImVec2(0, 0));
    }

    ImGui::SameLine();

    if (!this->model().outport_.pins_.empty()) {
        ImGui::BeginGroup();
        for (auto output : this->model().outport_.pins_) {
            ImNodes::BeginOutputAttribute(output->id_);
            // ImGui::Indent(40);
            ImGui::TextUnformatted(output->name_.c_str());
            ImNodes::EndOutputAttribute();
        }
        ImGui::EndGroup();
    } else {
        ImGui::Dummy(ImVec2(0, 0));
    }

    ImNodes::EndNode();

    // Always pull the latest ImNodes position back into the model so
    // serialization sees current values. Cheap; runs every frame.
    // (Pulled here rather than at editor level so each widget owns
    // its own state.)
    ImVec2 gp = ImNodes::GetNodeGridSpacePos(this->model().id_);
    this->grid_position_ = ModuleWidget::FromImVec2(gp);
}

DEFINE_MODEL_WIDGET_FACTORY(ModuleWidget, Module)

} // namespace augr