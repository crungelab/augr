#pragma once

#include <augr/rack/rack_doc.h>

#include "../view/probe_view.h"

#include "model_viewer.h"

namespace augr {
class ProbeViewer : public ModelViewerT<RackDoc, ProbeModule, ProbeView> {
public:
    using ModelViewerT<RackDoc, ProbeModule, ProbeView>::ModelViewerT;
};

} // namespace augr