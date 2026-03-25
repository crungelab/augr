#include "imgui.h"

#include "widget.h"
#include <augr/core/rack/control/h_slider.h>

namespace augr {

class HSliderWidget : public WidgetT<HSlider> {
public:
    HSliderWidget(HSlider &model) : WidgetT<HSlider>(model) {
        model_->zone_ = model_->zone_ ? model_->zone_ : &model_->init_;
    }
    void Draw() override {

        ImGui::SliderFloat(model_->label_, model_->zone_, model_->min_,
                           model_->max_);

        /*
        ImGui::DragFloat(
            model_->label_,
            model_->zone_,
            //1.0f,
            model_->step_,
            model_->min_,
            model_->max_);
        */
    }
};
DEFINE_WIDGET_FACTORY(HSliderWidget, HSlider)

} // namespace augr