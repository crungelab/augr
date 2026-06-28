#pragma once

#include <augr/rack/rack_doc.h>
#include "../view/console_view.h"
#include "viewer.h"

namespace augr {

class ModuleViewer : public ViewerT<RackDoc, Module, ConsoleView> {
public:
    using ViewerT<RackDoc, Module, ConsoleView>::ViewerT;

    void RebuildView() override;
};

} // namespace augr