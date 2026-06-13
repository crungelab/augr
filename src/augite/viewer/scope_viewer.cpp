#include "viewer_factory.h"

#include "scope_viewer.h"

namespace augr {

void ScopeViewer::Create() {
    Widget::Create();
    RebuildView();
}

void ScopeViewer::RebuildView() {
    view_ = std::make_unique<ScopeView>(model());
    view().set_model(model());
    view().Build();
}

DEFINE_VIEWER_FACTORY(ScopeViewer, RackDoc, ScopeModule)

} // namespace augr
