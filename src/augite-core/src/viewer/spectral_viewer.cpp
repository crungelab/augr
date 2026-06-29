#include <augite/viewer/viewer_factory.h>

#include <augite/viewer/spectral_viewer.h>

namespace augr {

void SpectralViewer::RebuildView() {
    view_ = std::make_unique<SpectralView>(model());
    view().Build();
}

DEFINE_VIEWER_FACTORY(SpectralViewer, RackDoc, SpectralModule)

} // namespace augr
