#include "imgui.h"
#include "imnodes.h"

#include "../widget/widget_builder.h"

#include "model_view.h"

namespace augr {

void ModelView::Build() {
    ModelWidgetBuilder builder;
    //root_ = builder.Build(*model());
    root_ = new Widget(); // dummy root to hold the real root's children
    AddChild(root_);
    builder.BuildChildren(*root_, *model_);
}

} // namespace augr