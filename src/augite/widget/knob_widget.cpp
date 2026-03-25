#include <imgui-knobs.h>

#include "widget.h"
#include <augr/core/rack/control/knob.h>

namespace augr {

class KnobWidget : public WidgetT<Knob> {
public:
  KnobWidget(Knob& model) : WidgetT<Knob>(model) {}
  void Draw() override {
    //ImGuiKnobs::Knob(model_->label_, model_->zone_, model_->min_, model_->max_, 0.1f, "%.1fdB", ImGuiKnobVariant_Wiper);
    ImGuiKnobs::Knob(model_->label_, model_->zone_, model_->min_, model_->max_, model_->step_, "%.1fdB", ImGuiKnobVariant_Wiper);
  }
};
DEFINE_WIDGET_FACTORY(KnobWidget, Knob)

} // namespace augr