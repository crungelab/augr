#include "imgui.h"

#include "widget.h"
#include <augr/rack/control/num_entry.h>

namespace augr {

class NumEntryWidget : public WidgetT<NumEntry> {
public:
  NumEntryWidget(NumEntry& model) : WidgetT<NumEntry>(model) {}
  void Draw() override {
    ImGui::InputFloat(model_->label_, model_->zone_);
  }
};
DEFINE_WIDGET_FACTORY(NumEntryWidget, NumEntry)

} // namespace augr