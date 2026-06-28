#pragma once

#include <augr/rack/library/probe_module.h>

#include "console_view.h"

namespace augr {

class ProbeView : public ModelViewT<ProbeModule, ConsoleView> {
public:
    using ModelViewT<ProbeModule, ConsoleView>::ModelViewT;
    void Draw() override;
};

} // namespace augr