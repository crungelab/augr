#pragma once

#include "inspector.h"

namespace augr {

class Module;

class ModuleInspector : public ModelWidgetT<Module> {
public:
    ModuleInspector(Module &model) : ModelWidgetT<Module>(model) {}
    void Draw() override;
};

} // namespace augr