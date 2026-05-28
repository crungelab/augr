#include <imgui.h>
#include <imgui-knobs.h>

#include "widget.h"
#include <augr/core/ui/control/knob.h>

namespace augr {

class KnobWidget : public ModelWidgetT<Knob> {
public:
    KnobWidget(Knob &model) : ModelWidgetT<Knob>(model) {}

    void Draw() override {
        auto &m = model();
        FloatParameter *param = m.param();
        float value = static_cast<float>(param->value());

        auto min = static_cast<float>(param->min());
        auto max = static_cast<float>(param->max());

        const float range = static_cast<float>(param->max() - param->min());
        float speed =
            range / 100.f; // 200px for full sweep regardless of range

        ImGuiKeyChord mods = ImGui::GetIO().KeyMods;
        if (mods & ImGuiMod_Alt)
            speed *= 2.0;
        else if ((mods & (ImGuiMod_Shift | ImGuiMod_Ctrl)) ==
                 (ImGuiMod_Shift | ImGuiMod_Ctrl))
            speed *= 0.0001;
        else if (mods & ImGuiMod_Ctrl)
            speed *= 0.01;
        else if (mods & ImGuiMod_Shift)
            speed *= 0.1;

        ImGuiKnobs::Knob(param->label().c_str(), &value, min, max, speed,
                         param->Format().c_str(), ImGuiKnobVariant_WiperDot,
                         0.f, ImGuiKnobFlags_AlwaysClamp);
        param->set_value(static_cast<fy_real>(value));

        if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
            param->ResetToInit();
    }
};
DEFINE_MODEL_WIDGET_FACTORY(KnobWidget, Knob)

/*
class KnobWidget : public ModelWidgetT<Knob> {
public:
    KnobWidget(Knob &model) : ModelWidgetT<Knob>(model) {}

    void Draw() override {
        FloatParameter *param = model_->param();
        float pos = static_cast<float>(param->GetNormalized());

        if (ImGuiKnobs::Knob(param->label().c_str(), &pos, 0.f, 1.f, 0.01f,
                             param->Format().c_str(), ImGuiKnobVariant_WiperDot,
                             0.f))
            param->SetNormalized(static_cast<fy_real>(pos));

        if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
            param->ResetToInit();
    }
};
DEFINE_MODEL_WIDGET_FACTORY(KnobWidget, Knob)
*/

} // namespace augr