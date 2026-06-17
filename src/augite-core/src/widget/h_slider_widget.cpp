#include "imgui.h"

#include <augite/widget/widget.h>
#include <augr/ui/control/h_slider.h>

namespace augr {

class HSliderWidget : public ModelWidgetT<HSlider> {
public:
    HSliderWidget(HSlider &model) : ModelWidgetT<HSlider>(model) {}

    void Draw() override {
        auto &m = model();
        FloatParameter *param = m.param();
        float pos = static_cast<float>(param->GetNormalized());

        if (ImGui::SliderFloat(param->label().c_str(), &pos, 0.f, 1.f, ""))
            param->SetNormalized(static_cast<fy_real>(pos));

        if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
            param->ResetToInit();

        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("%s", param->Format().c_str());
    }
};
DEFINE_MODEL_WIDGET_FACTORY(HSliderWidget, HSlider)

} // namespace augr