#include "imgui.h"
#include "imnodes.h"

#include <augr/model.h>

#include <augite/widget/widget_builder.h>
#include <augite/view/console_view.h>

namespace augr {

void ConsoleView::Build() {
    ModelWidgetBuilder builder;
    builder.BuildChildren(*this, *model().console_); // builds container with descendants only
}

} // namespace augr