#include <imgui-knobs.h>

#include "widget.h"
#include <augr/core/ui/control/knob.h>

namespace augr {

class KnobWidget : public WidgetT<Knob> {
public:
    KnobWidget(Knob &model) : WidgetT<Knob>(model) {}

    void Draw() override {
        FloatParameter *param = model_->param();
        float pos = static_cast<float>(param->GetNormalized());

        if (ImGuiKnobs::Knob(param->label().c_str(), &pos, 0.f, 1.f, 0.f,
                             param->Format().c_str(), ImGuiKnobVariant_WiperDot,
                             0.f))
            param->SetNormalized(static_cast<fy_real>(pos));

        if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
            param->ResetToInit();
    }
};
DEFINE_WIDGET_FACTORY(KnobWidget, Knob)

} // namespace augr