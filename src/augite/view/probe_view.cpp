#include <algorithm>

#include <imgui.h>

#include <augr/rack/library/probe_module.h>

#include "probe_view.h"

namespace augr {

void ProbeView::Draw() {
    auto &m = model();

    const float peak = m.Snapshot();
    // Meter bar still clamps for display, but make the clip threshold and the
    // numeric readout reflect the true magnitude — the bar pegging at 1.0 does
    // NOT mean the signal is 1.0.
    const float fraction = std::clamp(peak, 0.f, 1.f);
    const bool clipping = peak >= 1.f;

    ImGui::Text("Peak Level");
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram,
        clipping ? ImVec4(0.85f, 0.2f, 0.2f, 1.f)
                 : ImVec4(0.2f, 0.8f, 0.3f, 1.f));
    ImGui::ProgressBar(fraction, ImVec2(-1.f, 0.f));
    ImGui::PopStyleColor();
    // The actual peak, which may exceed 1.0 — this is the number that matters.
    ImGui::Text("peak = %.3f%s", peak, peak > 1.f ? "  (>1.0!)" : "");

    ImGui::Separator();
    ModuleView::Draw();
}

/*
void ProbeView::Draw() {
    auto &m = model();

    const float peak = m.Snapshot();
    const float fraction = std::clamp(peak, 0.f, 1.f);
    const bool clipping = peak >= 1.f;

    ImGui::Text("Peak Level");
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram,
        clipping ? ImVec4(0.85f, 0.2f, 0.2f, 1.f)
                 : ImVec4(0.2f, 0.8f, 0.3f, 1.f));
    ImGui::ProgressBar(fraction, ImVec2(-1.f, 0.f));
    ImGui::PopStyleColor();
    ImGui::Text("%.3f", peak);

    ImGui::Separator();
    ModuleView::Draw(); // draw parameters below the meter display
}
*/

} // namespace augr