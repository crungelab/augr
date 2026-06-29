#pragma once

#include <augr/rack/rack_doc.h>

#include "../view/dexie_view.h"

#include <augite/viewer/model_viewer.h>

namespace augr {
class DexieViewer : public ModelViewerT<RackDoc, fm::Dexie, DexieView> {
public:
    using ModelViewerT<RackDoc, fm::Dexie, DexieView>::ModelViewerT;
};

} // namespace augr