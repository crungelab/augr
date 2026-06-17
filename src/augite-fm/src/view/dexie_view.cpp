#include <algorithm>

#include <imgui.h>

#include <augr/fm/dexie.h>

#include <augite/fm/view/dexie_view.h>

namespace augr {

void DexieView::Draw() {
    auto &m = model();

    ImGui::Text("Freq: %.2f Hz  (ratio_coarse=%.2f  ratio_fine=%.3f)",
                m.debug_freq_, m.ratio_coarse_, m.ratio_fine_);

    const float peak = m.phase_mod_peak_;
    m.phase_mod_peak_ = 0.0f;
    ImGui::Text("Phase Mod peak: %.3f", peak);

    ModuleView::Draw();
}

} // namespace augr