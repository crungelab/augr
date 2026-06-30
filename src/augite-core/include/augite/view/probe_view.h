#pragma once

#include <augr/rack/library/probe_module.h>

#include "console_view.h"

namespace augr {

class ProbeView : public ViewT<ProbeModule, ConsoleView> {
public:
    using ViewT<ProbeModule, ConsoleView>::ViewT;
    void Draw() override;
};

} // namespace augr