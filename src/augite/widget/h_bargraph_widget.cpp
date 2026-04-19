#include <string>

#include "implot.h"
#include "widget.h"
#include <augr/core/ui/control/h_bargraph.h>

namespace augr {

class HBarGraphWidget : public WidgetT<HBarGraph> {
public:
    HBarGraphWidget(HBarGraph &model)
        : WidgetT<HBarGraph>(model) {}

    void Draw() override {
        Parameter *param = model_->param();
        float t = static_cast<float>(param->GetNormalized());

        std::string overlay = param->label() + ": " + param->Format();
        ImGui::ProgressBar(t, ImVec2(-FLT_MIN, 0), overlay.c_str());
    }
};
DEFINE_WIDGET_FACTORY(HBarGraphWidget, HBarGraph)

} // namespace augr