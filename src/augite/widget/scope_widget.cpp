#include "scope_widget.h"

#include <algorithm>
#include <vector>

#include <implot.h>

#include <augr/rack/library/scope_module.h>

namespace augr {

void ScopeWidget::DrawContent() {
    // Pull the latest window of samples from the module's ring buffer.
    const std::size_t window = static_cast<std::size_t>(
        std::clamp(model_->window_samples_, 128.f, 8192.f));

    static thread_local std::vector<float> display;
    display.resize(window);
    const std::size_t n = model_->Snapshot(display.data(), display.size());

    // Trigger alignment: find the first rising edge through trigger_level_
    // and compute a sub-sample offset so the waveform locks visually.
    std::size_t offset = 0;
    float frac = 0.f;
    if (n > 1) {
        const float level = model_->trigger_level_;
        for (std::size_t i = 1; i < n; ++i) {
            if (display[i - 1] < level && display[i] >= level) {
                offset = i;
                const float denom = display[i] - display[i - 1];
                frac = (denom != 0.f) ? (level - display[i - 1]) / denom : 0.f;
                break;
            }
        }
    }

    const int plot_count = static_cast<int>(n - offset);

    if (ImPlot::BeginPlot("##scope", ImVec2(-1, 160),
                          ImPlotFlags_CanvasOnly | ImPlotFlags_NoMouseText)) {
        ImPlot::SetupAxes(nullptr, nullptr,
                          ImPlotAxisFlags_NoDecorations |
                              ImPlotAxisFlags_NoMenus,
                          ImPlotAxisFlags_NoDecorations |
                              ImPlotAxisFlags_NoMenus | ImPlotAxisFlags_Lock);
        ImPlot::SetupAxisLimits(ImAxis_X1, -1.0,
                                static_cast<double>(std::max(plot_count, 1)),
                                ImPlotCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1, -1.1, 1.1, ImPlotCond_Always);

        if (plot_count > 0) {
            ImPlot::PlotLine("##sig", display.data() + offset, plot_count,
                             1.0,                         // xscale
                             static_cast<double>(-frac)); // xstart
        }

        ImPlot::EndPlot();
    }
}

DEFINE_WIDGET_FACTORY(ScopeWidget, ScopeModule)

} // namespace augr