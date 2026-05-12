#include "imgui.h"

#include "widget.h"
#include <augr/core/ui/control/combo.h>

namespace augr {

class ComboWidget : public ModelWidgetT<Combo> {
public:
    ComboWidget(Combo &model) : ModelWidgetT<Combo>(model) {}

    void Draw() {
        auto *param = model_->param();
        const int current = param->CurrentIndex();
        const char *preview =
            (current >= 0 && current < static_cast<int>(param->size()))
                ? param->LabelAt(current).c_str()
                : "";

        // ImGui requires a unique ID; the parameter's label serves.
        if (ImGui::BeginCombo(param->label().c_str(), preview)) {
            for (int i = 0; i < static_cast<int>(param->size()); ++i) {
                const bool selected = (i == current);
                if (ImGui::Selectable(param->LabelAt(i).c_str(), selected)) {
                    param->SetIndex(i);
                }
                if (selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        // Right-click to reset to default.
        if (ImGui::BeginPopupContextItem()) {
            if (ImGui::MenuItem("Reset to default")) {
                param->ResetToInit();
            }
            ImGui::EndPopup();
        }
    }
};

DEFINE_MODEL_WIDGET_FACTORY(ComboWidget, Combo)

} // namespace augr