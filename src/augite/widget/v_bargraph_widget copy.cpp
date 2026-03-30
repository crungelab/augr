#include <algorithm>
#include <string>
#include <cmath>
#include <cfloat>
#include <cstring> // memcpy for atomic-like load

#include "imgui.h"
#include "imgui_internal.h" // ImGuiWindow, ImDrawList

#include "widget.h"
#include <augr/core/rack/control/v_bargraph.h>

namespace augr {

// ---------- helpers (local to this translation unit) ----------

static std::string fmtValue(float v) {
    // If you use dB ranges (-60..0), append " dB" if you like.
    return std::to_string(v);
}

// Light heuristic: treat ranges like [-96..0], [-60..0], etc. as dB meters.
inline bool IsLikelyDbRange(float vmin, float vmax) {
    return (vmax <= 0.0f) && (vmin < 0.0f) && (vmin > -200.0f);
}

// If your Faust value is std::atomic<float>, prefer .load() instead of this.
// This memcpy trick avoids tearing for plain float in practice.
inline float atomic_like_load(const float* p) {
    float v;
    std::memcpy(&v, p, sizeof(float));
    return v;
}

// Map dB to 0..1 with a floor for responsiveness (default -60..0 dB).
inline float DbToUnit(float db, float floorDB = -60.0f, float ceilDB = 0.0f) {
    if (db <= floorDB) return 0.0f;
    if (db >= ceilDB)  return 1.0f;
    float lin      = std::pow(10.0f, db * 0.05f);          // 20*log10(x) => x = 10^(db/20)
    float linFloor = std::pow(10.0f, floorDB * 0.05f);
    float linCeil  = std::pow(10.0f, ceilDB  * 0.05f);
    return (lin - linFloor) / (linCeil - linFloor);
}

inline ImU32 MeterAutoColor(float unit) {
    if (unit < 0.7f) return ImGui::GetColorU32(ImVec4(0.20f, 0.9f, 0.20f, 1.0f)); // green
    if (unit < 0.9f) return ImGui::GetColorU32(ImVec4(0.95f, 0.85f, 0.15f, 1.0f)); // yellow
    return ImGui::GetColorU32(ImVec4(0.95f, 0.20f, 0.15f, 1.0f));                  // red
}

struct VBarPeakState {
    float peakUnit = 0.0f; // track in 0..1 space for simplicity
};

inline void UpdatePeak(VBarPeakState& s, float unitNow, float dt, float fallRatePerSec = 0.7f) {
    s.peakUnit = std::max(s.peakUnit - fallRatePerSec * dt, unitNow);
}

// Draw a compact vertical bar graph (bottom->top fill).
// - label: text drawn below (also used for ID/hover tip)
// - zone: Faust-updated value (float*)
// - vmin/vmax: Faust range (linear or dB)
// - size: bar size in pixels
// - use_db_mapping: interpret raw value as dB (use DbToUnit mapping)
inline void DrawVBargraphCore(const char* label,
                              const float* zone,
                              float vmin, float vmax,
                              ImVec2 size,
                              bool use_db_mapping,
                              VBarPeakState* peakOpt)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return;

    ImGui::BeginGroup();

    // Reserve rect
    const ImVec2 pos = ImGui::GetCursorScreenPos();
    const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
    ImGui::ItemSize(size);
    if (!ImGui::ItemAdd(bb, ImGui::GetID(label))) {
        ImGui::TextUnformatted(label);
        ImGui::EndGroup();
        return;
    }

    // Colors
    ImDrawList* dl = ImGui::GetWindowDrawList();
    const ImGuiStyle& style = ImGui::GetStyle();
    const float rounding = style.FrameRounding;
    const ImU32 col_bg     = ImGui::GetColorU32(style.Colors[ImGuiCol_FrameBg]);
    const ImU32 col_border = ImGui::GetColorU32(style.Colors[ImGuiCol_Border]);

    // Read raw and normalize
    const float raw = atomic_like_load(zone);
    float unit = 0.0f;
    if (use_db_mapping) {
        // You can tie the floor/ceil to vmin/vmax if desired:
        // unit = DbToUnit(raw, vmin, vmax);
        unit = DbToUnit(raw, -60.0f, 0.0f);
    } else {
        const float denom = (vmax != vmin) ? (vmax - vmin) : 1.0f;
        unit = (raw - vmin) / denom;
        unit = std::clamp(unit, 0.0f, 1.0f);
    }

    // Frame
    dl->AddRectFilled(bb.Min, bb.Max, col_bg, rounding);
    dl->AddRect(bb.Min, bb.Max, col_border, rounding);

    // Inner area + bar
    const float pad = 2.0f;
    ImRect inner(ImVec2(bb.Min.x + pad, bb.Min.y + pad),
                 ImVec2(bb.Max.x - pad, bb.Max.y - pad));

    const float fill_h = unit * inner.GetHeight();
    ImRect fillRect(ImVec2(inner.Min.x, inner.Max.y - fill_h),
                    ImVec2(inner.Max.x, inner.Max.y));

    const ImU32 col_fill = MeterAutoColor(unit);
    dl->AddRectFilled(fillRect.Min, fillRect.Max, col_fill, rounding);

    // Tick marks (25%, 50%, 75%)
    for (int i = 1; i < 4; ++i) {
        float y = inner.Max.y - inner.GetHeight() * (i / 4.0f);
        dl->AddLine(ImVec2(inner.Min.x, y), ImVec2(inner.Min.x + 6.0f, y),
                    ImGui::GetColorU32(ImVec4(1,1,1,0.25f)));
    }

    // Peak-hold line (optional)
    if (peakOpt) {
        float dt = ImGui::GetIO().DeltaTime;
        UpdatePeak(*peakOpt, unit, dt);
        float ph = std::clamp(peakOpt->peakUnit, 0.0f, 1.0f) * inner.GetHeight();
        float y = inner.Max.y - ph;
        dl->AddLine(ImVec2(inner.Min.x, y), ImVec2(inner.Max.x, y),
                    ImGui::GetColorU32(ImVec4(1,1,1,0.9f)), 2.0f);
    }

    // Tooltip w/ your formatter
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        if (label && *label) ImGui::TextUnformatted(label);
        ImGui::Separator();
        ImGui::TextUnformatted(fmtValue(raw).c_str());
        ImGui::EndTooltip();
    }

    // Label under the meter
    ImGui::SetCursorScreenPos(ImVec2(pos.x,
        pos.y + size.y + style.ItemInnerSpacing.y * 0.5f));
    if (label && *label) ImGui::TextUnformatted(label);

    ImGui::EndGroup();
}

// ---------- your widget ----------

class VBarGraphWidget : public WidgetT<VBarGraph> {
public:
    VBarGraphWidget(VBarGraph &model) : WidgetT<VBarGraph>(model) {}

    void Draw() override {
        // Heuristic: use dB meter mapping when the range looks like dB.
        const bool as_db = IsLikelyDbRange(model_->min_, model_->max_);

        // Width/height: choose something compact; tweak to taste or make configurable on model.
        const ImVec2 size(22.0f, 120.0f);

        DrawVBargraphCore(
            model_->label_ ? model_->label_ : "vbar",
            model_->zone_,
            model_->min_, model_->max_,
            size,
            as_db,
            &peak_ // Pass &peak_ for a subtle peak-hold line; pass nullptr to disable.
        );
    }

private:
    VBarPeakState peak_;
};

DEFINE_WIDGET_FACTORY(VBarGraphWidget, VBarGraph)

} // namespace augr
