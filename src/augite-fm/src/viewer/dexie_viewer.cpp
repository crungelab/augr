#include <augite/viewer/viewer_factory.h>

#include <augite/fm/viewer/dexie_viewer.h>

namespace augr {

void DexieViewer::RebuildView() {
    view_ = std::make_unique<DexieView>(model());
    view().Build();
}

DEFINE_VIEWER_FACTORY(DexieViewer, RackDoc, fm::Dexie)

} // namespace augr
