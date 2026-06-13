#pragma once

#include <augr/rack/module/module.h>

#include "model_view.h"

namespace augr {

class ModuleView : public ModelViewT<Module> {
public:
    using ModelViewT<Module>::ModelViewT;
};

} // namespace augr