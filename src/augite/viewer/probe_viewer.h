#pragma once

#include <augr/rack/rack_doc.h>

#include "../view/probe_view.h"

#include "viewer.h"

namespace augr {
class ProbeViewer : public ViewerT<RackDoc, ProbeModule, ProbeView> {
public:
    using ViewerT<RackDoc, ProbeModule, ProbeView>::ViewerT;

    void RebuildView() override;
};

} // namespace augr