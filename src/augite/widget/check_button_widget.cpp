#include "imgui.h"

#include "widget.h"
#include <augr/core/control/check_button.h>

namespace augr {

class CheckButtonWidget : public WidgetT<CheckButton> {
public:
    CheckButtonWidget(CheckButton &model) : WidgetT<CheckButton>(model) {}

    void Draw() override {
        Parameter *param = model_->param();
        bool checked = param->GetValue() != fy_real{0};

        if (ImGui::Checkbox(param->label().c_str(), &checked))
            param->SetValue(checked ? param->max() : param->min());
    }
};
DEFINE_WIDGET_FACTORY(CheckButtonWidget, CheckButton)

} // namespace augr