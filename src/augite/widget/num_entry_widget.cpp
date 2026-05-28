#include "imgui.h"

#include "widget.h"
#include <augr/core/ui/control/num_entry.h>

namespace augr {

class NumEntryWidget : public ModelWidgetT<NumEntry> {
public:
    NumEntryWidget(NumEntry &model) : ModelWidgetT<NumEntry>(model) {}

    void Draw() override {
        auto &m = model();
        FloatParameter *param = m.param();
        float value = static_cast<float>(param->value());

        if (ImGui::InputFloat(param->label().c_str(), &value, 0.f, 0.f,
                              param->Format().c_str()))
            param->set_value(static_cast<fy_real>(value));
    }
};

DEFINE_MODEL_WIDGET_FACTORY(NumEntryWidget, NumEntry)

} // namespace augr