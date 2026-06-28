#include "imgui.h"
#include "imnodes.h"

#include <augr/model.h>

#include <augite/widget/widget_builder.h>
#include <augite/view/console_view.h>

namespace augr {

void ConsoleView::Build() {
    ModelWidgetBuilder builder;
    root_ = new Widget(); // dummy root to hold the real root's children
    AddChild(Widget::Ptr(root_)); // take ownership of the dummy root
    builder.BuildChildren(*root_, *model().console_); // builds container with descendants only
}

} // namespace augr