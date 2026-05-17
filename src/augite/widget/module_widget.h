#pragma once

#include <string>

#include <imgui.h>
#include <imnodes.h>

#include "widget.h"

namespace augr {

class Module;

class ModuleWidget : public ModelWidget {
public:
    void ShowWindow() { is_open_ = true; }
    void HideWindow() { is_open_ = false; }

    // Data members
    bool is_open_ = false;
    std::string window_name_;
};

template <typename T> class ModuleWidgetT : public ModelWidgetT<T, ModuleWidget> {
public:
    explicit ModuleWidgetT(T &model) : ModelWidgetT<T, ModuleWidget>(model) {
        this->window_name_ = std::string(this->model_->label_) + "###module_" +
                             std::to_string(this->model_->id_);
    }
    void Draw() override {
        DrawNode();

        if (this->is_open_) {
            DrawWindow();
        }
    }

    void DrawWindow() {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));

        if (ImGui::Begin(this->window_name_.c_str(), &this->is_open_)) {
            ImGui::PushID(this->model_->id_);
            this->DrawContent();
            ImGui::PopID();
        }

        ImGui::End();
        ImGui::PopStyleVar();
    }

    virtual void DrawContent() {
        this->DrawChildren();
    }

    void DrawNode() {
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
};

using DefaultModuleWidget = ModuleWidgetT<Module>;

} // namespace augr