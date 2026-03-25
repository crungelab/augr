#include "imgui.h"

#include "box_widget.h"
#include <augr/rack/control/h_box.h>

namespace augr {

// Match VBox knobs
struct HBoxStyle {
    bool framed = true;
    bool collapsible = false;
    bool default_open = true;
    bool show_inner_vseps = false; // vertical separators between children
    float inner_padding = 6.0f;
};

class HBoxWidget : public BoxWidgetT<HBox> {
public:
    HBoxWidget(HBox &model) : BoxWidgetT<HBox>(model) {}

    void Draw() override {
        HBoxStyle style_cfg{};
        ImGui::PushID(model_);
        const char *label =
            (model_->label_ && *model_->label_) ? model_->label_ : nullptr;

        /*
        if (style_cfg.collapsible && label) {
            DrawAsCollapsible(label, style_cfg);
        } else if (style_cfg.framed) {
            DrawAsFramedPanel(label, style_cfg);
        } else {
            DrawAsPlainRow(label, style_cfg);
        }
        */
        if (!model_->is_top_level_) {
            DrawAsFramedPanel(label, style_cfg);
        } else {
            DrawAsPlainRow(label, style_cfg);
        }

        ImGui::PopID();
    }

private:
    // ---- Core: draw children in a stretch-equal table (no SameLine, no
    // SetCursor) ----
    void DrawChildrenInTable(bool show_inner_vseps) {
        const int cols = (int)children_.size();
        if (cols <= 0)
            return;

        ImGuiTableFlags tf =
            ImGuiTableFlags_SizingStretchSame |
            ImGuiTableFlags_NoSavedSettings |
            ImGuiTableFlags_BordersInnerV * (show_inner_vseps ? 1 : 0);

        if (ImGui::BeginTable("hbox_table", cols, tf, ImVec2(-FLT_MIN, 0))) {
            ImGui::TableNextRow();
            for (int c = 0; c < cols; ++c) {
                ImGui::TableSetColumnIndex(c);
                ImGui::PushID(children_[c]);
                // Each child draws “normally”; the column constrains its width.
                ImGui::BeginGroup();
                children_[c]->Draw();
                ImGui::EndGroup();
                ImGui::PopID();
            }
            ImGui::EndTable();
        }
    }

    // ---- Plain row (no frame) ----
    void DrawAsPlainRow(const char *label, const HBoxStyle & /*style*/) {
        ImGuiStyle &st = ImGui::GetStyle();
        ImGui::BeginGroup();
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,
                            ImVec2(st.ItemSpacing.x, st.ItemSpacing.y * 0.6f));

        DrawHeader();
        /*
        if (label) {
            ImGui::TextUnformatted(label);
            ImGui::Separator(); // ok to use built-in here
            ImGui::Dummy(ImVec2(0.0f, st.ItemSpacing.y)); // extra space below
        }
        */

        DrawChildrenInTable(/*show_inner_vseps=*/false);

        ImGui::PopStyleVar();
        ImGui::EndGroup();
        ImGui::Dummy(ImVec2(0.0f, st.ItemSpacing.y));
    }

    // ---- Collapsible header ----
    void DrawAsCollapsible(const char *label, const HBoxStyle &style) {
        ImGuiTreeNodeFlags flags =
            style.default_open ? ImGuiTreeNodeFlags_DefaultOpen : 0;
        if (ImGui::CollapsingHeader(label, flags)) {
            ImGuiStyle &st = ImGui::GetStyle();
            ImGui::Indent();
            ImGui::PushStyleVar(
                ImGuiStyleVar_ItemSpacing,
                ImVec2(st.ItemSpacing.x, st.ItemSpacing.y * 0.6f));
            DrawChildrenInTable(/*show_inner_vseps=*/false);
            ImGui::PopStyleVar();
            ImGui::Unindent();
        }
    }

    // ---- Framed panel (title + in-frame separator + border behind content)
    // ----
    void DrawAsFramedPanel(const char *label, const HBoxStyle &style) {
        ImDrawList *dl = ImGui::GetWindowDrawList();
        ImGuiStyle &st = ImGui::GetStyle();
        const float pad = style.inner_padding;
        const float r = st.FrameRounding;

        const ImVec2 outer_min = ImGui::GetCursorScreenPos();

        // Split channels so frame is behind content
        // dl->ChannelsSplit(2);
        dl_splitter_.Split(dl, 2);
        // dl->ChannelsSetCurrent(1); // content
        dl_splitter_.SetCurrentChannel(dl, 1); // content

        // Inner padded region where we’ll clip title + table so they stay
        // inside the frame
        const ImVec2 inner_min(outer_min.x + pad, outer_min.y + pad);
        const float inner_right = ImGui::GetWindowPos().x +
                                  ImGui::GetWindowContentRegionMax().x - pad;

        ImGui::SetCursorScreenPos(inner_min);
        ImGui::BeginGroup();
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,
                            ImVec2(st.ItemSpacing.x, st.ItemSpacing.y * 0.6f));

        // Clip everything (title, separator, table) to the inner frame width
        ImGui::PushClipRect(ImVec2(inner_min.x, inner_min.y),
                            ImVec2(inner_right, FLT_MAX), true);

        /*
        float sep_y = 0.0f;
        if (label) {
            ImGui::TextUnformatted(label);
            ImGui::Separator();                           // clipped → won’t
        leak past frame
            // add a bit more space than default under the title line
            ImGui::Dummy(ImVec2(0.0f, st.ItemSpacing.y * 1.25f));
            // record Y for completeness (not strictly needed since we used
        Separator) sep_y = ImGui::GetCursorScreenPos().y;
        }
        */
        DrawHeader();

        DrawChildrenInTable(/*show_inner_vseps=*/style.show_inner_vseps);

        ImGui::PopClipRect();
        ImGui::PopStyleVar();
        ImGui::EndGroup();

        // Measure content to size the frame
        const ImVec2 content_min = ImGui::GetItemRectMin(); // ≈ inner_min
        const ImVec2 content_max = ImGui::GetItemRectMax();
        const ImVec2 outer_max(content_max.x + pad, content_max.y + pad);

        // Draw frame behind content
        // dl->ChannelsSetCurrent(0);
        dl_splitter_.SetCurrentChannel(dl, 0);
        dl->AddRectFilled(outer_min, outer_max,
                          ImGui::GetColorU32(ImGuiCol_FrameBg), r);
        dl->AddRect(outer_min, outer_max, ImGui::GetColorU32(ImGuiCol_Border),
                    r);
        // dl->ChannelsMerge();
        dl_splitter_.Merge(dl);

        // Advance after the frame
        ImGui::SetCursorScreenPos(ImVec2(outer_min.x, outer_max.y));
        ImGui::Dummy(ImVec2(0.0f, st.ItemSpacing.y));
    }
};

DEFINE_WIDGET_FACTORY(HBoxWidget, HBox)

} // namespace augr
