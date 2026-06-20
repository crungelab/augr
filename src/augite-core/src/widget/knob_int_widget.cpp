#include <imgui-knobs.h>
#include <imgui.h>

#include <augite/widget/widget.h>
#include <augr/ui/control/knob.h>

namespace augr {

class KnobIntWidget : public ModelWidgetT<KnobInt> {
public:
    KnobIntWidget(KnobInt &model) : ModelWidgetT<KnobInt>(model) {}

    void Draw() override {
        auto &m = model();
        IntParameter *param = m.param();
        auto value = param->value();

        auto min = static_cast<float>(param->min());
        auto max = static_cast<float>(param->max());

        const float range = static_cast<float>(param->max() - param->min());
        float speed = range / 100.f; // 200px for full sweep regardless of range

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

        if (ImGuiKnobs::KnobInt(param->label().c_str(), &value, min, max, speed,
                             param->Format().c_str(), ImGuiKnobVariant_WiperDot,
                             0.f, ImGuiKnobFlags_AlwaysClamp)) {
            param->set_value(static_cast<fy_real>(value));
        }

        if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
            param->ResetToInit();
    }
};
DEFINE_MODEL_WIDGET_FACTORY(KnobIntWidget, KnobInt)

} // namespace augr