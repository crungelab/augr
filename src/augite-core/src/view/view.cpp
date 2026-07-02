#include "imgui.h"
#include "imnodes.h"

#include <augite/widget/widget_builder.h>

#include <augite/view/view.h>

namespace augr {

void View::Build() {
    ModelWidgetBuilder builder;
    builder.BuildChildren(*this, *model_);
}

} // namespace augr