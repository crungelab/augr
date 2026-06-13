#include "viewer_factory.h"

#include "spectral_viewer.h"

namespace augr {

void SpectralViewer::RebuildView() {
    view_ = std::make_unique<SpectralView>(model());
    view().set_model(model());
    view().Build();
}

DEFINE_VIEWER_FACTORY(SpectralViewer, RackDoc, SpectralModule)

} // namespace augr
