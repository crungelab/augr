#include "imgui.h"
#include "imnodes.h"

#include <augite/widget/widget_builder.h>

#include <augite/view/model_view.h>

namespace augr {

void ModelView::Build() {
    ModelWidgetBuilder builder;
    root_ = new Widget(); // dummy root to hold the real root's children
    AddChild(Widget::Ptr(root_)); // take ownership of the dummy root
    builder.BuildChildren(*root_, *model_);
}

} // namespace augr