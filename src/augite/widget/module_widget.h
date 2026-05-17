#pragma once

#include <string>

#include <imgui.h>
#include <imnodes.h>

#include <augr/core/math/vec2.h>

#include "widget.h"

namespace augr {

class Module;

class ModuleWidget : public ModelWidget {
public:
    void ShowWindow() { is_open_ = true; }
    void HideWindow() { is_open_ = false; }

    // Conversion helpers — keep Vec2 (ImGui-free) at the data layer
    // and only touch ImVec2 at the draw boundary.
    static ImVec2 ToImVec2(const Vec2 &v) { return ImVec2(v.x, v.y); }
    static Vec2 FromImVec2(const ImVec2 &v) { return Vec2{v.x, v.y}; }

    // Data members
    bool is_open_ = false;
    std::string window_name_;
    //
    Vec2 grid_position_ = {0.0f, 0.0f};
    Vec2 window_position_ = {100.0f, 100.0f};
    Vec2 window_size_ = {320.0f, 240.0f};

    // Set to true when state has been loaded from JSON or explicitly
    // assigned (e.g. catalog spawn), telling Draw to push the values
    // into ImNodes/ImGui on the next frame. Cleared after the push.
    bool position_dirty_ = true;
    bool window_pose_dirty_ = true;
};

template <typename T>
class ModuleWidgetT : public ModelWidgetT<T, ModuleWidget> {
public:
    explicit ModuleWidgetT(T &model) : ModelWidgetT<T, ModuleWidget>(model) {
        this->window_name_ = std::string(this->model_->label_) + "###module_" +
                             std::to_string(this->model_->id_);
    }

    void Draw() override {
        DrawNode();

        // Always pull the latest ImNodes position back into the model so
        // serialization sees current values. Cheap; runs every frame.
        // (Pulled here rather than at editor level so each widget owns
        // its own state.)
        ImVec2 gp = ImNodes::GetNodeGridSpacePos(this->model_->id_);
        this->grid_position_ = ModuleWidget::FromImVec2(gp);

        if (this->is_open_) {
            DrawWindow();
        }
    }

    void DrawNode() {
        // Push persisted position on the first draw after load/spawn.
        if (this->position_dirty_) {
            ImNodes::SetNodeGridSpacePos(
                this->model_->id_,
                ModuleWidget::ToImVec2(this->grid_position_));
            this->position_dirty_ = false;
        }

        ImNodes::BeginNode(this->model_->id_);

        ImNodes::BeginNodeTitleBar();
        ImGui::TextUnformatted(this->model_->label_.c_str());
        ImNodes::EndNodeTitleBar();

        for (auto input : this->model_->inport_.pins_) {
            ImNodes::BeginInputAttribute(input->id_);
            ImGui::TextUnformatted(input->name_.c_str());
            ImNodes::EndInputAttribute();
        }

        for (auto output : this->model_->outport_.pins_) {
            ImNodes::BeginOutputAttribute(output->id_);
            ImGui::Indent(40);
            ImGui::TextUnformatted(output->name_.c_str());
            ImNodes::EndOutputAttribute();
        }

        ImNodes::EndNode();
    }

    void DrawWindow() {
        // Push persisted window pose on the first draw after load/open.
        if (this->window_pose_dirty_) {
            ImGui::SetNextWindowPos(
                ModuleWidget::ToImVec2(this->window_position_));
            ImGui::SetNextWindowSize(
                ModuleWidget::ToImVec2(this->window_size_));
            this->window_pose_dirty_ = false;
        }

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));

        if (ImGui::Begin(this->window_name_.c_str(), &this->is_open_)) {
            ImGui::PushID(this->model_->id_);
            this->DrawContent();
            ImGui::PopID();

            // Pull current pose back so the next save sees latest.
            this->window_position_ = ModuleWidget::FromImVec2(ImGui::GetWindowPos());
            this->window_size_ = ModuleWidget::FromImVec2(ImGui::GetWindowSize());
        }

        ImGui::End();
        ImGui::PopStyleVar();
    }

    virtual void DrawContent() { this->DrawChildren(); }
};

using DefaultModuleWidget = ModuleWidgetT<Module>;

} // namespace augr