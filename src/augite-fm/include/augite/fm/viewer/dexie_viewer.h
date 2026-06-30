#pragma once

#include <augr/rack/rack_doc.h>

#include "../view/dexie_view.h"

#include <augite/viewer/viewer.h>

namespace augr {
class DexieViewer : public ViewerT<RackDoc, fm::Dexie, DexieView> {
public:
    using ViewerT<RackDoc, fm::Dexie, DexieView>::ViewerT;
};

} // namespace augr