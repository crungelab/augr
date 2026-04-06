#include "imgui.h"

#include "widget.h"
#include <augr/core/control/num_entry.h>

namespace augr {

class NumEntryWidget : public WidgetT<NumEntry> {
public:
  NumEntryWidget(NumEntry& model) : WidgetT<NumEntry>(model) {}
  void Draw() override {
    float value = model_->value();
    ImGui::InputFloat(model_->label_.c_str(), &value);
    model_->set_value(value);
  }
};
DEFINE_WIDGET_FACTORY(NumEntryWidget, NumEntry)

} // namespace augr