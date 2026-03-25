#include "imgui.h"
#include "imnodes.h"

#include "../widget/widget_builder.h"

#include "model_view.h"

namespace augr {

void ModelView::Build() {
  WidgetBuilder builder;
  root_ = builder.Build(*model());
}

} // namespace augr