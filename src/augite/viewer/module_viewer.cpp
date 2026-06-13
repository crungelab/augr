#include <augr/core/archiver_factory.h>

#include "../archiver/viewer_archiver.h"

#include "viewer_factory.h"

#include "module_viewer.h"

namespace augr {

void ModuleViewer::Create() {
    Widget::Create();
    RebuildView();
}

void ModuleViewer::RebuildView() {
    view_ = std::make_unique<ModuleView>(model());
    view().set_model(model());
    view().Build();
}

DEFINE_VIEWER_FACTORY(ModuleViewer, RackDoc, Module)

class ModuleViewerArchiver : public ViewerArchiver {};
DEFINE_ARCHIVER_FACTORY(ModuleViewerArchiver, ModuleViewer, "ModuleViewer")

} // namespace augr
