#include "imgui.h"

#include "widget.h"
#include <augr/core/control/h_slider.h>

namespace augr {

class HSliderWidget : public WidgetT<HSlider> {
public:
    HSliderWidget(HSlider &model) : WidgetT<HSlider>(model) {
        // model_->zone_ = model_->zone_ ? model_->zone_ : &model_->init_;
        model_->set_value(model_->value() ? model_->value() : model_->init_);
    }
    void Draw() override {

        float value = model_->value();
        ImGui::SliderFloat(model_->label_.c_str(), &value, model_->min_,
                           model_->max_);
        model_->set_value(value);
    }
};
DEFINE_WIDGET_FACTORY(HSliderWidget, HSlider)

} // namespace augr