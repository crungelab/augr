#pragma once

#include "module_widget.h"

namespace augr {

class ScopeModule;

class ScopeWidget : public ModuleWidgetT<ScopeModule> {
public:
    explicit ScopeWidget(ScopeModule& model) : ModuleWidgetT<ScopeModule>(model) {}
};

} // namespace augr