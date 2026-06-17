#include <augite/viewer/viewer_factory.h>

#include <augite/viewer/probe_viewer.h>

namespace augr {

void ProbeViewer::RebuildView() {
    view_ = std::make_unique<ProbeView>(model());
    view().set_model(model());
    view().Build();
}

DEFINE_VIEWER_FACTORY(ProbeViewer, RackDoc, ProbeModule)

} // namespace augr
