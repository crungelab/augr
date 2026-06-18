// #include "imgui.h"
// #include "imnodes.h"

#include <augr/rack/module/module.h>

#include <augite/widget/module_widget.h>

namespace augr {

void ModuleWidget::Draw() {
    DrawNode();

    // Always pull the latest ImNodes position back into the model so
    // serialization sees current values. Cheap; runs every frame.
    // (Pulled here rather than at editor level so each widget owns
    // its own state.)
    ImVec2 gp = ImNodes::GetNodeGridSpacePos(this->model().id_);
    this->grid_position_ = ModuleWidget::FromImVec2(gp);
    /*
    if (this->is_open_) {
        DrawViewer();
    }
    */
}

void ModuleWidget::DrawNode() {
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

    /*
    bool muted = model().muted_;
    if (ImGui::Checkbox("Mute", &muted))
        model().muted_ = muted;
    */
    for (auto input : this->model().inport_.pins_) {
        ImNodes::BeginInputAttribute(input->id_);
        ImGui::TextUnformatted(input->name_.c_str());
        ImNodes::EndInputAttribute();
    }

    DrawNodeContent();

    for (auto output : this->model().outport_.pins_) {
        ImNodes::BeginOutputAttribute(output->id_);
        ImGui::Indent(40);
        ImGui::TextUnformatted(output->name_.c_str());
        ImNodes::EndOutputAttribute();
    }

    ImNodes::EndNode();
}

/*
void ModuleWidget::DrawViewer() {
    // Push persisted window pose on the first draw after load/open.
    if (this->window_pose_dirty_) {
        ImGui::SetNextWindowPos(ModuleWidget::ToImVec2(this->window_position_));
        ImGui::SetNextWindowSize(ModuleWidget::ToImVec2(this->window_size_));
        this->window_pose_dirty_ = false;
    }

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));

    if (ImGui::Begin(this->window_name_.c_str(), &this->is_open_)) {
        ImGui::PushID(this->model_->id_);
        this->DrawView();
        ImGui::PopID();

        // Pull current pose back so the next save sees latest.
        this->window_position_ =
            ModuleWidget::FromImVec2(ImGui::GetWindowPos());
        this->window_size_ = ModuleWidget::FromImVec2(ImGui::GetWindowSize());
    }

    ImGui::End();
    ImGui::PopStyleVar();
}
*/

DEFINE_MODEL_WIDGET_FACTORY(ModuleWidget, Module)

} // namespace augr