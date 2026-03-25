#pragma once

#include "widget.h"

namespace augr {

class Module;

class ModuleWidget : public WidgetT<Module> {
public:
    ModuleWidget(Module &model) : WidgetT<Module>(model) {}
    void Draw() override;
    virtual void DrawNode();
    virtual void DrawWindow();

    void ShowWindow() { is_open_ = true; }
    void HideWindow() { is_open_ = false; }
    
    // Data members
    bool is_open_ = false;
};

} // namespace augr