#include <algorithm>
#include <string>

// #include "imgui.h"
#include "implot.h"

#include "widget.h"
#include <augr/core/rack/control/h_bargraph.h>

namespace augr {

static std::string fmtValue(float v) {
    // If you use dB ranges (-60..0), append " dB"
    return std::to_string(v);
}

class HBarGraphWidget : public WidgetT<HBarGraph> {
public:
    HBarGraphWidget(HBarGraph &model) : WidgetT<HBarGraph>(model) {}
    void Draw() override {
        float v = *model_->zone_; // see thread-safety note below
        float t = 0.0f;
        if (model_->max_ > model_->min_)
            t = (v - model_->min_) / (model_->max_ - model_->min_);
        t = std::clamp(t, 0.0f, 1.0f);

        // Expand to full width (-FLT_MIN), auto height
        std::string overlay =
            model_->label_ ? (std::string(model_->label_) + ": " + fmtValue(v))
                           : fmtValue(v);
        ImGui::ProgressBar(t, ImVec2(-FLT_MIN, 0), overlay.c_str());

        /*
        ImGui::PlotHistogram(model_->label_, model_->zone_, 1, 0, NULL,
            model_->min_, model_->max_, ImVec2(0, 80.0f));
        */

        /*
        if (ImPlot::BeginPlot(model_->label_)) {
            ImPlot::PlotBars<fy_real>(model_->label_, model_->zone_, 1, 0.67, 0,
                                      ImPlotBarsFlags_Horizontal);
            ImPlot::EndPlot();
        }
        */
    }
};
DEFINE_WIDGET_FACTORY(HBarGraphWidget, HBarGraph)

} // namespace augr