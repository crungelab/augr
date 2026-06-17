#include <string>

#include "implot.h"
#include <augite/widget/widget.h>
#include <augr/ui/control/h_bargraph.h>

namespace augr {

class HBarGraphWidget : public ModelWidgetT<HBarGraph> {
public:
    HBarGraphWidget(HBarGraph &model)
        : ModelWidgetT<HBarGraph>(model) {}

    void Draw() override {
        auto &m = model();
        FloatParameter *param = m.param();
        float t = static_cast<float>(param->GetNormalized());

        std::string overlay = param->label() + ": " + param->Format();
        ImGui::ProgressBar(t, ImVec2(-FLT_MIN, 0), overlay.c_str());
    }
};
DEFINE_MODEL_WIDGET_FACTORY(HBarGraphWidget, HBarGraph)

} // namespace augr