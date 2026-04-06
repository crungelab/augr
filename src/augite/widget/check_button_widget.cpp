#include "imgui.h"

#include "widget.h"
#include <augr/core/control/check_button.h>

namespace augr {

class CheckButtonWidget : public WidgetT<CheckButton> {
public:
    CheckButtonWidget(CheckButton &model) : WidgetT<CheckButton>(model) {}
    void Draw() override {
        bool value = model_->value() != 0.0f;
        bool changed = ImGui::Checkbox(model_->label_.c_str(), &value);
        if (changed)
            model_->set_value(value ? 1.0f : 0.0f);
    }
};
DEFINE_WIDGET_FACTORY(CheckButtonWidget, CheckButton)

} // namespace augr