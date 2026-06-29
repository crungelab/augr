#pragma once

#include <augr/rack/rack_doc.h>
#include "../view/console_view.h"
#include "model_viewer.h"

namespace augr {

class ModuleViewer : public ModelViewerT<RackDoc, Module, ConsoleView> {
public:
    using ModelViewerT<RackDoc, Module, ConsoleView>::ModelViewerT;
};

} // namespace augr