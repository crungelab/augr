#include "imgui.h"

#include "widget.h"
#include <augr/core/ui/control/v_slider.h>

namespace augr {

class VSliderWidget : public ModelWidgetT<VSlider> {
public:
    VSliderWidget(VSlider& model) : ModelWidgetT<VSlider>(model) {}

    void Draw() override {
        auto &m = model();
        ImVec2 size(64, 128);
        FloatParameter* param = m.param();

        float pos = static_cast<float>(param->GetNormalized());

        if (ImGui::VSliderFloat(param->label().c_str(), size, &pos, 0.f, 1.f, ""))
            param->SetNormalized(static_cast<fy_real>(pos));

        if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
            param->ResetToInit();

        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("%s", param->Format().c_str());
    }
};
DEFINE_MODEL_WIDGET_FACTORY(VSliderWidget, VSlider)

} // namespace augr