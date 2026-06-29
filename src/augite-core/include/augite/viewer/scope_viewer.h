#pragma once

#include <augr/rack/rack_doc.h>

#include "../view/scope_view.h"

#include "model_viewer.h"

namespace augr {
class ScopeViewer : public ModelViewerT<RackDoc, ScopeModule, ScopeView> {
public:
    using ModelViewerT<RackDoc, ScopeModule, ScopeView>::ModelViewerT;
};

} // namespace augr