#pragma once

#include <augr/rack/rack_doc.h>

#include "../view/scope_view.h"

#include "viewer.h"

namespace augr {
class ScopeViewer : public ViewerT<RackDoc, ScopeModule, ScopeView> {
public:
    using ViewerT<RackDoc, ScopeModule, ScopeView>::ViewerT;

    void RebuildView() override;
};

} // namespace augr