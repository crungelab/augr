#pragma once

#include <augr/rack/rack_doc.h>

#include "../view/spectral_view.h"

#include "model_viewer.h"

namespace augr {
class SpectralViewer : public ModelViewerT<RackDoc, SpectralModule, SpectralView> {
public:
    using ModelViewerT<RackDoc, SpectralModule, SpectralView>::ModelViewerT;
};

} // namespace augr