#include "imgui.h"

#include <augite/widget/model_widget.h>
#include <augr/ui/control/check_button.h>

namespace augr {

class CheckButtonWidget : public ModelWidgetT<CheckButton> {
public:
    CheckButtonWidget(CheckButton &model) : ModelWidgetT<CheckButton>(model) {}

    void Draw() override {
        auto &m = model();
        FloatParameter *param = m.param();
        bool checked = param->value() != fy_real{0};

        if (ImGui::Checkbox(param->label().c_str(), &checked))
            param->set_value(checked ? param->max() : param->min());
    }
};
DEFINE_MODEL_WIDGET_FACTORY(CheckButtonWidget, CheckButton)

class CheckButtonBoolWidget : public ModelWidgetT<CheckButtonBool> {
public:
    CheckButtonBoolWidget(CheckButtonBool &model) : ModelWidgetT<CheckButtonBool>(model) {}

    void Draw() override {
        auto &m = model();
        BoolParameter *param = m.param();
        bool checked = param->value();

        if (ImGui::Checkbox(param->label().c_str(), &checked))
            param->set_value(checked);
    }
};
DEFINE_MODEL_WIDGET_FACTORY(CheckButtonBoolWidget, CheckButtonBool)

} // namespace augr