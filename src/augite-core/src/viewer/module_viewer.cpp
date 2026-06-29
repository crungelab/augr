#include <augr/archiver_factory.h>

#include <augite/archiver/viewer_archiver.h>

#include <augite/viewer/viewer_factory.h>

#include <augite/viewer/module_viewer.h>

namespace augr {

DEFINE_VIEWER_FACTORY(ModuleViewer, RackDoc, Module)

class ModuleViewerArchiver : public ViewerArchiver {};
DEFINE_ARCHIVER_FACTORY(ModuleViewerArchiver, ModuleViewer, "ModuleViewer")

} // namespace augr
