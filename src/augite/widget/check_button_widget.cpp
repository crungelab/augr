#include "imgui.h"

#include "widget.h"
#include <augr/core/rack/control/check_button.h>

namespace augr {

class CheckButtonWidget : public WidgetT<CheckButton> {
public:
  CheckButtonWidget(CheckButton& model) : WidgetT<CheckButton>(model) {}
  void Draw() override {
    bool value = *model_->zone_ == 0.0 ? false : true;
    bool changed = ImGui::Checkbox(model_->label_, &value);
    if(changed)
      *model_->zone_ = value ? 1.0 : 0.0;
  }
};
DEFINE_WIDGET_FACTORY(CheckButtonWidget, CheckButton)

} // namespace augr