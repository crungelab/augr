#pragma once

#include "widget.h"

namespace augr {

class Module;

class ModuleWidget : public WidgetT<Module> {
public:
    ModuleWidget(Module &model) : WidgetT<Module>(model) {
        window_name_ = std::string(model_->label_) + "###module_" +
                       std::to_string(model_->id_);
    }
    void Draw() override;
    virtual void DrawNode();
    virtual void DrawWindow();

    void ShowWindow() { is_open_ = true; }
    void HideWindow() { is_open_ = false; }

    // Data members
    bool is_open_ = false;
    std::string window_name_;
};

} // namespace augr