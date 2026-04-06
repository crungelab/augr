#include "imgui.h"

#include "widget.h"
#include <augr/core/control/v_slider.h>

namespace augr {

class VSliderWidget : public WidgetT<VSlider> {
public:
  VSliderWidget(VSlider& model) : WidgetT<VSlider>(model) {}
  void Draw() override {
    ImVec2 size(64, 128);
    float value = model_->value();
    ImGui::VSliderFloat(model_->label_.c_str(), size, &value, model_->min_, model_->max_);
    model_->set_value(value);
  }
};
DEFINE_WIDGET_FACTORY(VSliderWidget, VSlider)

} // namespace augr