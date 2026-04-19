#include "imgui.h"

#include "widget.h"
#include <augr/core/ui/control/button.h>

namespace augr {

class ButtonWidget : public WidgetT<Button> {
public:
    ButtonWidget(Button &model) : WidgetT<Button>(model) {}

    void Draw() override {
        Parameter *param = model_->param();
        ImGui::Button(param->label().c_str());
        param->set_value(ImGui::IsItemActive() ? param->max() : param->min());
    }
};
DEFINE_WIDGET_FACTORY(ButtonWidget, Button)

} // namespace augr