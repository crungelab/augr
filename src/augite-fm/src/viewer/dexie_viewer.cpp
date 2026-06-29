#include <augite/viewer/viewer_factory.h>

#include <augite/fm/viewer/dexie_viewer.h>

namespace augr {

DEFINE_VIEWER_FACTORY(DexieViewer, RackDoc, fm::Dexie)

} // namespace augr
