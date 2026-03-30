#include <algorithm>
#include <cfloat>
#include <cmath>
#include <string>

#include <fmt/core.h>

#include "imgui.h"
#include "imgui_internal.h"

#include "widget.h"
#include <augr/core/rack/control/v_bargraph.h>

namespace augr {

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

// Map a raw dB value to 0..1 using a linear-in-dB scale (correct for meters).
// vmin/vmax come directly from the Faust bargraph registration.
inline float DbToUnit(float db, float vmin, float vmax) {
    const float range = vmax - vmin;
    if (range <= 0.0f)
        return 0.0f;
    return std::clamp((db - vmin) / range, 0.0f, 1.0f);
}

// Map a raw linear value to 0..1.
inline float LinearToUnit(float v, float vmin, float vmax) {
    const float range = vmax - vmin;
    if (range <= 0.0f)
        return 0.0f;
    return std::clamp((v - vmin) / range, 0.0f, 1.0f);
}

// Zone to unit, dispatching on whether the bargraph is dB-typed.
inline float ZoneToUnit(float raw, float vmin, float vmax, bool is_db) {
    return is_db ? DbToUnit(raw, vmin, vmax) : LinearToUnit(raw, vmin, vmax);
}

inline ImU32 MeterColor(float unit) {
    if (unit < 0.70f)
        return ImGui::GetColorU32(ImVec4(0.20f, 0.88f, 0.20f, 1.0f)); // green
    if (unit < 0.90f)
        return ImGui::GetColorU32(ImVec4(0.95f, 0.82f, 0.12f, 1.0f)); // yellow
    return ImGui::GetColorU32(ImVec4(0.95f, 0.18f, 0.12f, 1.0f));     // red
}

// Format a value for the tooltip.  Appends " dB" for dB-typed bars.
static std::string FormatZoneValue(float v, bool is_db) {
    char buf[32];
    if (is_db)
        std::snprintf(buf, sizeof(buf), "%.1f dB", v);
    else
        std::snprintf(buf, sizeof(buf), "%.3f", v);
    return buf;
}

// ---------------------------------------------------------------------------
// Peak-hold state
// ---------------------------------------------------------------------------

struct VBarPeakState {
    float unit = 0.0f; // tracked in normalised 0..1 space
    float hold = 0.0f; // seconds remaining at peak before decay starts

    static constexpr float kHoldSec = 1.2f;    // how long peak is held
    static constexpr float kFallPerSec = 0.5f; // normalised units / second
};

inline void UpdatePeak(VBarPeakState &s, float unitNow, float dt) {
    if (unitNow >= s.unit) {
        s.unit = unitNow;
        s.hold = VBarPeakState::kHoldSec;
    } else {
        s.hold -= dt;
        if (s.hold <= 0.0f)
            s.unit =
                std::max(s.unit - VBarPeakState::kFallPerSec * dt, unitNow);
    }
}

// ---------------------------------------------------------------------------
// Core draw routine
// ---------------------------------------------------------------------------

inline void DrawVBargraphCore(const char *label,
                              const float *zone, // live pointer into DSP field
                              float vmin, float vmax, bool is_db, ImVec2 size,
                              VBarPeakState *peak) {
    ImGuiWindow *window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return;

    ImGui::PushID(label);
    ImGui::BeginGroup();

    const ImGuiStyle &style = ImGui::GetStyle();
    ImDrawList *dl = ImGui::GetWindowDrawList();
    const float round = style.FrameRounding;

    // --- reserve rect ---
    const ImVec2 pos = ImGui::GetCursorScreenPos();
    const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
    ImGui::ItemSize(size);
    const ImGuiID id = ImGui::GetID("bar");
    if (!ImGui::ItemAdd(bb, id)) {
        ImGui::EndGroup();
        ImGui::PopID();
        return;
    }

    // --- read zone (direct float* read is safe for display purposes) ---
    const float raw = *zone;
    /*
    fmt::print("zone={} raw={} is_db={} vmin={} vmax={}\n", (void *)zone, raw,
               is_db, vmin, vmax);
    */
    const float unit = ZoneToUnit(raw, vmin, vmax, is_db);

    // --- background + border ---
    dl->AddRectFilled(bb.Min, bb.Max,
                      ImGui::GetColorU32(style.Colors[ImGuiCol_FrameBg]),
                      round);
    dl->AddRect(bb.Min, bb.Max,
                ImGui::GetColorU32(style.Colors[ImGuiCol_Border]), round);

    // --- inner fill area ---
    const float pad = 2.0f;
    const ImRect inner(ImVec2(bb.Min.x + pad, bb.Min.y + pad),
                       ImVec2(bb.Max.x - pad, bb.Max.y - pad));
    const float innerH = inner.GetHeight();

    // bar fill (bottom-to-top)
    const float fillH = unit * innerH;
    if (fillH > 0.0f) {
        const ImRect fillRect(ImVec2(inner.Min.x, inner.Max.y - fillH),
                              ImVec2(inner.Max.x, inner.Max.y));
        dl->AddRectFilled(fillRect.Min, fillRect.Max, MeterColor(unit),
                          round * 0.5f);
    }

    // tick marks at -12, -6, 0 dB (or 25%/50%/75% for linear)
    {
        const ImU32 tickCol = ImGui::GetColorU32(ImVec4(1, 1, 1, 0.20f));
        const float tickW = inner.GetWidth() * 0.35f;
        for (int i = 1; i <= 3; ++i) {
            const float frac = i / 4.0f;
            const float y = inner.Max.y - innerH * frac;
            dl->AddLine(ImVec2(inner.Min.x, y), ImVec2(inner.Min.x + tickW, y),
                        tickCol);
        }
    }

    // peak-hold line
    if (peak) {
        UpdatePeak(*peak, unit, ImGui::GetIO().DeltaTime);
        const float ph = std::clamp(peak->unit, 0.0f, 1.0f) * innerH;
        const float y = inner.Max.y - ph;
        dl->AddLine(ImVec2(inner.Min.x, y), ImVec2(inner.Max.x, y),
                    ImGui::GetColorU32(ImVec4(1, 1, 1, 0.85f)), 1.5f);
    }

    // --- tooltip ---
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort)) {
        ImGui::BeginTooltip();
        if (label && *label) {
            ImGui::TextUnformatted(label);
            ImGui::Separator();
        }
        ImGui::TextUnformatted(FormatZoneValue(raw, is_db).c_str());
        ImGui::EndTooltip();
    }

    // --- label clipped to bar width, centred below ---
    if (label && *label) {
        const float labelY = pos.y + size.y + style.ItemInnerSpacing.y;
        ImGui::SetCursorScreenPos(ImVec2(pos.x, labelY));
        // Push a clip rect so the label never spills outside the bar width.
        ImGui::PushClipRect(
            ImVec2(pos.x, labelY),
            ImVec2(pos.x + size.x, labelY + ImGui::GetTextLineHeight()), true);
        ImGui::TextUnformatted(label);
        ImGui::PopClipRect();
    }

    ImGui::EndGroup();
    ImGui::PopID();
}

// ---------------------------------------------------------------------------
// Widget
// ---------------------------------------------------------------------------

class VBarGraphWidget : public WidgetT<VBarGraph> {
public:
    explicit VBarGraphWidget(VBarGraph &model) : WidgetT<VBarGraph>(model) {}

    void Draw() override {
        // VBarGraph::is_db_ must be set by FaustDspUi when the "unit"="dB"
        // metadata is declared — this is the correct, reliable signal rather
        // than a heuristic on the range values.
        const bool is_db = model_->is_db_;
        const ImVec2 size(24.0f, 120.0f);

        DrawVBargraphCore(model_->label_ ? model_->label_ : "", model_->zone_,
                          model_->min_, model_->max_, is_db, size, &peak_);
    }

private:
    VBarPeakState peak_;
};

DEFINE_WIDGET_FACTORY(VBarGraphWidget, VBarGraph)

} // namespace augr