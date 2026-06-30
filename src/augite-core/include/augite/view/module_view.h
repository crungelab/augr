#pragma once

#include <augr/rack/module/module.h>

#include "view.h"

namespace augr {

class ModuleView : public ViewT<Module> {
public:
    using ViewT<Module>::ViewT;
};

} // namespace augr