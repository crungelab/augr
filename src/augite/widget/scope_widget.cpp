#include "scope_widget.h"

#include <algorithm>
#include <vector>

#include <implot.h>

#include <augr/rack/library/scope_module.h>

namespace augr {

void ScopeWidget::DrawView() {
    auto &m = model();
    const std::size_t display_window = static_cast<std::size_t>(
        std::clamp(m.window_samples_, 128.f, 8192.f));

    // Capture extra so the trigger search has room on both sides.
    const std::size_t capture_window = display_window * kCaptureMultiplier;

    static thread_local std::vector<float> capture;
    capture.resize(capture_window);
    const std::size_t n = m.Snapshot(capture.data(), capture.size());

    // Find the rising edge closest to the middle of the capture. Using the
    // middle (rather than the first crossing) keeps the trigger selection
    // stable frame to frame, even as the ring's write pointer advances.
    const float level = m.trigger_level_;
    const std::size_t search_center = n / kCaptureMultiplier;

    std::size_t best_edge = 0;
    std::size_t best_dist = n;
    float best_frac = 0.f;
    bool found = false;

    for (std::size_t i = 1; i < n; ++i) {
        if (capture[i - 1] < level && capture[i] >= level) {
            const std::size_t dist =
                (i > search_center) ? (i - search_center) : (search_center - i);
            if (dist < best_dist) {
                best_dist = dist;
                best_edge = i;
                const float denom = capture[i] - capture[i - 1];
                best_frac =
                    (denom != 0.f) ? (level - capture[i - 1]) / denom : 0.f;
                found = true;
            }
        }
    }

    // Resample onto a grid that's phase-locked to the sub-sample trigger
    // crossing. resampled[i] is the signal value at exactly i samples
    // after the true crossing, giving a stable display across frames.

    static thread_local std::vector<float> resampled;
    resampled.clear();

    if (found) {
        resampled.reserve(display_window);
        const float true_trigger_pos =
            static_cast<float>(best_edge - 1) + best_frac;

        for (std::size_t i = 0; i < display_window; ++i) {
            const float src = true_trigger_pos + static_cast<float>(i);
            const std::size_t i0 = static_cast<std::size_t>(src);
            const std::size_t i1 = i0 + 1;
            if (i1 >= n) break;
            const float t = src - static_cast<float>(i0);
            resampled.push_back(
                capture[i0] * (1.f - t) + capture[i1] * t);
        }
    }

    if (ImPlot::BeginPlot("##scope", ImVec2(-1, 160),
                          ImPlotFlags_CanvasOnly | ImPlotFlags_NoMouseText)) {
        /*
        ImPlot::SetupAxes(nullptr, nullptr,
                          ImPlotAxisFlags_NoDecorations |
                              ImPlotAxisFlags_NoMenus,
                          ImPlotAxisFlags_NoDecorations |
                              ImPlotAxisFlags_NoMenus | ImPlotAxisFlags_Lock);
        */

        // X-axis range is fixed to display_window (stable knob value),
        // not to resampled.size() (varies per frame). This keeps the
        // axis and tick marks from wobbling, which was causing ghosting.

        ImPlot::SetupAxisLimits(ImAxis_X1, 0.0,
                                static_cast<double>(display_window),
                                ImPlotCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1, -1.1, 1.1, ImPlotCond_Always);

        if (!resampled.empty()) {
            ImPlot::PlotLine("##sig", resampled.data(),
                             static_cast<int>(resampled.size()));
        }

        ImPlot::EndPlot();
    }
    ModuleWidgetT<ScopeModule>::DrawView(); // draw parameters below the scope display
}

DEFINE_MODEL_WIDGET_FACTORY(ScopeWidget, ScopeModule)

} // namespace augr