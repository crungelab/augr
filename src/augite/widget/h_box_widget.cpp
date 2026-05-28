#include "imgui.h"

#include "box_widget.h"
#include <augr/core/ui/control/h_box.h>

namespace augr {

class HBoxWidget : public BoxWidgetT<HBox> {
public:
    HBoxWidget(HBox &model) : BoxWidgetT<HBox>(model) {}

    void Draw() override {
        ImGui::PushID(model_);

        if (!model().is_top_level_) {
            DrawAsFramedPanel();
        } else {
            DrawAsPlainRow();
        }

        ImGui::PopID();
    }

private:
    static constexpr float kInnerPadding = 6.0f;

    // ---- Core: draw children in a stretch-equal table ----
    // column_padding: horizontal space between columns (applied as CellPadding.x,
    // so each side of a column gets half this value).
    void DrawChildrenInTable(float column_padding = 0.0f,
                             bool show_inner_vseps = false) {
        const int cols = static_cast<int>(children_.size());
        if (cols == 0)
            return;

        ImGuiTableFlags tf =
            ImGuiTableFlags_SizingStretchSame |
            ImGuiTableFlags_NoSavedSettings |
            (show_inner_vseps ? ImGuiTableFlags_BordersInnerV : 0);

        ImGuiStyle &st = ImGui::GetStyle();
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding,
                            ImVec2(column_padding * 0.5f, st.CellPadding.y));

        if (ImGui::BeginTable("hbox_table", cols, tf, ImVec2(-FLT_MIN, 0))) {
            ImGui::TableNextRow();
            for (int c = 0; c < cols; ++c) {
                ImGui::TableSetColumnIndex(c);
                ImGui::PushID(children_[c].get());
                ImGui::BeginGroup();
                children_[c]->Draw();
                ImGui::EndGroup();
                ImGui::PopID();
            }
            ImGui::EndTable();
        }

        ImGui::PopStyleVar();
    }

    // ---- Plain row (no frame, used for top-level boxes) ----
    void DrawAsPlainRow() {
        ImGuiStyle &st = ImGui::GetStyle();
        ImGui::BeginGroup();
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,
                            ImVec2(st.ItemSpacing.x, st.ItemSpacing.y * 0.6f));

        DrawHeader();
        DrawChildrenInTable();

        ImGui::PopStyleVar();
        ImGui::EndGroup();
        ImGui::Dummy(ImVec2(0.0f, st.ItemSpacing.y));
    }

    // ---- Framed panel (used for nested boxes) ----
    // Uses dl_splitter_ (inherited from BoxWidgetT) to draw the frame behind
    // the content.
    void DrawAsFramedPanel() {
        ImDrawList *dl = ImGui::GetWindowDrawList();
        ImGuiStyle &st = ImGui::GetStyle();
        const float pad = kInnerPadding;
        const float r = st.FrameRounding;

        const ImVec2 outer_min = ImGui::GetCursorScreenPos();

        // Split channels so frame is drawn behind content
        dl_splitter_.Split(dl, 2);
        dl_splitter_.SetCurrentChannel(dl, 1); // content channel

        // Inner padded region
        const ImVec2 inner_min(outer_min.x + pad, outer_min.y + pad);
        const float inner_right = ImGui::GetWindowPos().x +
                                  ImGui::GetWindowContentRegionMax().x - pad;

        ImGui::SetCursorScreenPos(inner_min);
        ImGui::BeginGroup();
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,
                            ImVec2(st.ItemSpacing.x, st.ItemSpacing.y * 0.6f));

        // Clip title + table to the inner frame width
        ImGui::PushClipRect(ImVec2(inner_min.x, inner_min.y),
                            ImVec2(inner_right, FLT_MAX), true);

        DrawHeader();
        DrawChildrenInTable();

        ImGui::PopClipRect();
        ImGui::PopStyleVar();
        ImGui::EndGroup();

        // Measure content to size the frame
        const ImVec2 content_max = ImGui::GetItemRectMax();
        const ImVec2 outer_max(content_max.x + pad, content_max.y + pad);

        // Draw frame behind content
        dl_splitter_.SetCurrentChannel(dl, 0); // background channel
        dl->AddRectFilled(outer_min, outer_max,
                          ImGui::GetColorU32(ImGuiCol_FrameBg), r);
        dl->AddRect(outer_min, outer_max, ImGui::GetColorU32(ImGuiCol_Border),
                    r);
        dl_splitter_.Merge(dl);

        // Advance past the frame
        ImGui::SetCursorScreenPos(ImVec2(outer_min.x, outer_max.y));
        ImGui::Dummy(ImVec2(0.0f, st.ItemSpacing.y));
    }
};

DEFINE_MODEL_WIDGET_FACTORY(HBoxWidget, HBox)

} // namespace augr