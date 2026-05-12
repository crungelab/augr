#include "imgui.h"

#include "widget.h"
#include <augr/core/ui/control/check_button.h>

namespace augr {

class CheckButtonWidget : public ModelWidgetT<CheckButton> {
public:
    CheckButtonWidget(CheckButton &model) : ModelWidgetT<CheckButton>(model) {}

    void Draw() override {
        FloatParameter *param = model_->param();
        bool checked = param->value() != fy_real{0};

        if (ImGui::Checkbox(param->label().c_str(), &checked))
            param->set_value(checked ? param->max() : param->min());
    }
};
DEFINE_MODEL_WIDGET_FACTORY(CheckButtonWidget, CheckButton)

} // namespace augr