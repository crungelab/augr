#include "imgui.h"

#include <augite/widget/model_widget.h>
#include <augr/ui/control/button.h>

namespace augr {

class ButtonWidget : public ModelWidgetT<Button> {
public:
    ButtonWidget(Button &model) : ModelWidgetT<Button>(model) {}

    void Draw() override {
        auto &m = model();
        FloatParameter *param = m.param();
        ImGui::Button(param->label().c_str());
        param->set_value(ImGui::IsItemActive() ? param->max() : param->min());
    }
};
DEFINE_MODEL_WIDGET_FACTORY(ButtonWidget, Button)

} // namespace augr