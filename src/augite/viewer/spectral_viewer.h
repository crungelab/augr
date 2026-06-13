#pragma once

#include <augr/rack/rack_doc.h>

#include "../view/spectral_view.h"

#include "viewer.h"

namespace augr {
class SpectralViewer : public ViewerT<RackDoc, SpectralModule, SpectralView> {
public:
    using ViewerT<RackDoc, SpectralModule, SpectralView>::ViewerT;
    void Create() override;

    virtual void RebuildView();
};

} // namespace augr