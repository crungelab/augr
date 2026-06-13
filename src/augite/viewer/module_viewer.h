#pragma once

#include <augr/rack/rack_doc.h>
#include "../view/module_view.h"
#include "viewer.h"

namespace augr {

class ModuleViewer : public ViewerT<RackDoc, Module, ModuleView> {
public:
    using ViewerT<RackDoc, Module, ModuleView>::ViewerT;

    void RebuildView() override;
};

} // namespace augr