#pragma once

#include <augr/rack/library/probe_module.h>

#include "module_view.h"

namespace augr {

class ProbeView : public ModelViewT<ProbeModule, ModuleView> {
public:
    using ModelViewT<ProbeModule, ModuleView>::ModelViewT;
    void Draw() override;
};

} // namespace augr