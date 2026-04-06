#include <imgui-knobs.h>

#include "widget.h"
#include <augr/core/control/knob.h>

namespace augr {

class KnobWidget : public WidgetT<Knob> {
public:
  KnobWidget(Knob& model) : WidgetT<Knob>(model) {}
  void Draw() override {
    //ImGuiKnobs::Knob(model_->label_, model_->zone_, model_->min_, model_->max_, 0.1f, "%.1fdB", ImGuiKnobVariant_Wiper);
    float value = model_->value();
    ImGuiKnobs::Knob(model_->label_.c_str(), &value, model_->min_, model_->max_, model_->step_, "%.1fdB", ImGuiKnobVariant_Wiper);
    model_->set_value(value);
  }
};
DEFINE_WIDGET_FACTORY(KnobWidget, Knob)

} // namespace augr