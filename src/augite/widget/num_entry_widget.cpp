#include "imgui.h"

#include "widget.h"
#include <augr/core/control/num_entry.h>

namespace augr {

class NumEntryWidget : public WidgetT<NumEntry> {
public:
    NumEntryWidget(NumEntry &model) : WidgetT<NumEntry>(model) {}

    void Draw() override {
        Parameter *param = model_->param();
        float value = static_cast<float>(param->GetValue());

        if (ImGui::InputFloat(param->label().c_str(), &value, 0.f, 0.f, param->Format().c_str()))
            param->SetValue(static_cast<fy_real>(value));
    }
};
DEFINE_WIDGET_FACTORY(NumEntryWidget, NumEntry)

} // namespace augr