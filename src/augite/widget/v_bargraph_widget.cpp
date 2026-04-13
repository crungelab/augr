#include <algorithm>
#include <cfloat>
#include <cmath>
#include <string>

#include <fmt/core.h>

#include "imgui.h"
#include "imgui_internal.h"

#include "widget.h"
#include <augr/core/control/v_bargraph.h>

namespace augr {

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

inline ImU32 MeterColor(float unit) {
    if (unit < 0.70f)
        return ImGui::GetColorU32(ImVec4(0.20f, 0.88f, 0.20f, 1.0f));
    if (unit < 0.90f)
        return ImGui::GetColorU32(ImVec4(0.95f, 0.82f, 0.12f, 1.0f));
    return ImGui::GetColorU32(ImVec4(0.95f, 0.18f, 0.12f, 1.0f));
}

// ---------------------------------------------------------------------------
// Peak-hold state
// ---------------------------------------------------------------------------

struct VBarPeakState {
    float unit = 0.0f;
    float hold = 0.0f;

    static constexpr float kHoldSec = 1.2f;
    static constexpr float kFallPerSec = 0.5f;
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
// unit: pre-normalized 0..1 value (caller owns the conversion)
// raw:  internal value for tooltip display via Parameter::Format()

inline void DrawVBargraphCore(const char *label, float unit,
                              const std::string &formatted, ImVec2 size,
                              VBarPeakState *peak) {
    ImGuiWindow *window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return;

    ImGui::PushID(label);
    ImGui::BeginGroup();

    const ImGuiStyle &style = ImGui::GetStyle();
    ImDrawList *dl = ImGui::GetWindowDrawList();
    const float round = style.FrameRounding;

    const ImVec2 pos = ImGui::GetCursorScreenPos();
    const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
    ImGui::ItemSize(size);
    const ImGuiID id = ImGui::GetID("bar");
    if (!ImGui::ItemAdd(bb, id)) {
        ImGui::EndGroup();
        ImGui::PopID();
        return;
    }

    // background + border
    dl->AddRectFilled(bb.Min, bb.Max,
                      ImGui::GetColorU32(style.Colors[ImGuiCol_FrameBg]),
                      round);
    dl->AddRect(bb.Min, bb.Max,
                ImGui::GetColorU32(style.Colors[ImGuiCol_Border]), round);

    // inner fill
    const float pad = 2.0f;
    const ImRect inner(ImVec2(bb.Min.x + pad, bb.Min.y + pad),
                       ImVec2(bb.Max.x - pad, bb.Max.y - pad));
    const float innerH = inner.GetHeight();

    const float fillH = unit * innerH;
    if (fillH > 0.0f) {
        const ImRect fillRect(ImVec2(inner.Min.x, inner.Max.y - fillH),
                              ImVec2(inner.Max.x, inner.Max.y));
        dl->AddRectFilled(fillRect.Min, fillRect.Max, MeterColor(unit),
                          round * 0.5f);
    }

    // tick marks
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

    // tooltip
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort)) {
        ImGui::BeginTooltip();
        if (label && *label) {
            ImGui::TextUnformatted(label);
            ImGui::Separator();
        }
        ImGui::TextUnformatted(formatted.c_str());
        ImGui::EndTooltip();
    }

    // label below bar
    if (label && *label) {
        const float labelY = pos.y + size.y + style.ItemInnerSpacing.y;
        ImGui::SetCursorScreenPos(ImVec2(pos.x, labelY));
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
    explicit VBarGraphWidget(VBarGraph &model)
        : WidgetT<VBarGraph>(model) {}

    void Draw() override {
        Parameter *param = model_->param();
        const float unit = static_cast<float>(param->GetNormalized());
        const ImVec2 size(24.0f, 120.0f);

        DrawVBargraphCore(param->label().c_str(), unit, param->Format(), size,
                          &peak_);
    }

private:
    VBarPeakState peak_;
};

DEFINE_WIDGET_FACTORY(VBarGraphWidget, VBarGraph)

} // namespace augr