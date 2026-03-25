#include "imgui.h"

#include <augr/core/rack/control/v_box.h>

#include "box_widget.h"

namespace augr {

// Optional: extend your model with a tiny style/config (or fetch from metadata)
struct VBoxStyle {
    bool framed = true;       // draw a bordered panel
    bool collapsible = false; // use a CollapsingHeader instead of a frame
    bool default_open = true; // initial state if collapsible
    // float inner_padding = 6.0f; // padding inside the group
    float inner_padding = 10.0f; // padding inside the group
};

class VBoxWidget : public BoxWidgetT<VBox> {
public:
    VBoxWidget(VBox &model) : BoxWidgetT<VBox>(model) {}

    void Draw() override {
        // You can wire this up to metadata on your VBox if you have it
        VBoxStyle style_cfg{};
        // e.g. read from model_->zone_ / metadata if your Faust wrapper exposes
        // it style_cfg.framed = model_->is_framed(); style_cfg.collapsible =
        // model_->is_collapsible();
        // ...

        ImGui::PushID(model_); // ensure stable IDs even with duplicate labels

        const char *label =
            (model_->label_ && *model_->label_) ? model_->label_ : nullptr;

        /*
        if (style_cfg.collapsible) {
            DrawAsCollapsible(label, style_cfg);
        } else if (style_cfg.framed) {
            DrawAsFramedPanel(label, style_cfg);
        } else {
            DrawAsPlainGroup(label, style_cfg);
        }
        */
        if (!model_->is_top_level_) {
            DrawAsFramedPanel(label, style_cfg);
        } else {
            DrawAsPlainGroup(label, style_cfg);
        }

        ImGui::PopID();
    }

private:
    void DrawTitleLine_(const char *label) {
        if (!label)
            return;
        // A small title line feels nice in a panel; SeparatorText also works
        ImGui::TextUnformatted(label);
        ImGui::Separator();
    }

    void DrawAsPlainGroup(const char *label, const VBoxStyle &style_cfg) {
        ImGui::BeginGroup();
        /*
        if (label) {
            // Keep it light: separator title then stack children
            ImGui::SeparatorText(label);
        }
        */
        DrawHeader();
        // Slightly tighter inner spacing looks better for stacked controls
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,
                            ImVec2(ImGui::GetStyle().ItemSpacing.x,
                                   ImGui::GetStyle().ItemSpacing.y * 0.6f));
        DrawChildren();
        ImGui::PopStyleVar();
        ImGui::EndGroup();
    }

    void DrawAsCollapsible(const char *label, const VBoxStyle &style_cfg) {
        // CollapsingHeader uses the label as the clickable header
        // If no label, fall back to a plain group
        if (!label) {
            DrawAsPlainGroup(nullptr, style_cfg);
            return;
        }
        ImGuiTreeNodeFlags flags =
            ImGuiTreeNodeFlags_DefaultOpen * (style_cfg.default_open ? 1 : 0);
        if (ImGui::CollapsingHeader(label, flags)) {
            ImGui::Indent(); // small visual nesting
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,
                                ImVec2(ImGui::GetStyle().ItemSpacing.x,
                                       ImGui::GetStyle().ItemSpacing.y * 0.6f));
            DrawChildren();
            ImGui::PopStyleVar();
            ImGui::Unindent();
        }
    }

    void DrawAsFramedPanel(const char *label, const VBoxStyle &style_cfg) {
        ImDrawList *dl = ImGui::GetWindowDrawList();
        ImGuiStyle &st = ImGui::GetStyle();

        const float pad = style_cfg.inner_padding;
        const float r = st.FrameRounding;

        // Outer top-left where we'll paint the frame later
        const ImVec2 outer_min = ImGui::GetCursorScreenPos();

        // Split channels so frame is behind content
        // dl->ChannelsSplit(2);
        dl_splitter_.Split(dl, 2);
        // dl->ChannelsSetCurrent(1); // content
        dl_splitter_.SetCurrentChannel(dl, 1); // content

        // Move cursor to inner content origin (respect top/left padding)
        const ImVec2 inner_min(outer_min.x + pad, outer_min.y + pad);
        ImGui::SetCursorScreenPos(inner_min);

        // Build content in a group to get its bounding box later
        ImGui::BeginGroup();
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,
                            ImVec2(st.ItemSpacing.x, st.ItemSpacing.y * 0.6f));

        // We'll draw the separator AFTER measuring content width, but we
        // need to remember the Y where it should go.
        /*
        float sep_y = 0.0f;

        if (label && *label) {
            // Draw label now; separator will be inserted right below it
            ImGui::TextUnformatted(label);
            // Record the separator Y based on current cursor (right below
            // label) Do not draw yet; we need the group's width first.
            sep_y = ImGui::GetCursorScreenPos().y + st.ItemSpacing.y * 0.25f;
            // Add a tiny vertical gap after the future separator line
            // (we'll also add a Dummy when we actually draw it)
        }
        */
        DrawHeader();

        // Draw children controls
        DrawChildren();

        ImGui::PopStyleVar();
        ImGui::EndGroup();

        // Measure the content rect of the group we just drew
        const ImVec2 content_min =
            ImGui::GetItemRectMin(); // equals inner_min if group non-empty
        const ImVec2 content_max = ImGui::GetItemRectMax();

        // Compute outer frame rect by inflating content rect with padding
        const ImVec2 outer_max(content_max.x + pad, content_max.y + pad);

        // Draw the separator now that we know the true content width
        /*
        if (label && *label) {
            dl->AddLine(ImVec2(content_min.x, sep_y),
                        ImVec2(content_max.x, sep_y),
                        ImGui::GetColorU32(ImGuiCol_Separator), 1.0f);
            // Provide the post-separator vertical spacing (place cursor under
            // the line) Move cursor to "just below the separator" based on
            // measured X min
            ImGui::SetCursorScreenPos(ImVec2(content_min.x, sep_y));
            ImGui::Dummy(ImVec2(0.0f, st.ItemSpacing.y * 0.75f));
        }
        */

        // Now draw the frame on the background channel, so it sits behind
        // content
        // dl->ChannelsSetCurrent(0);
        dl_splitter_.SetCurrentChannel(dl, 0);
        dl->AddRectFilled(outer_min, outer_max,
                          ImGui::GetColorU32(ImGuiCol_FrameBg), r);
        dl->AddRect(outer_min, outer_max, ImGui::GetColorU32(ImGuiCol_Border),
                    r);
        // dl->ChannelsMerge();
        dl_splitter_.Merge(dl);

        // Advance cursor to just after the frame so the next widget stacks
        // nicely
        ImGui::SetCursorScreenPos(ImVec2(outer_min.x, outer_max.y));
        ImGui::Dummy(ImVec2(0.0f, st.ItemSpacing.y));
    }

    /*
    void DrawAsFramedPanel(const char* label, const VBoxStyle& style_cfg) {
        // A bordered child region that auto-fills width; height grows with
content
        // Use a distinct child ID per vgroup to avoid conflicts
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,
ImVec2(style_cfg.inner_padding, style_cfg.inner_padding));
        // Note: Auto-resize Y is version-dependent; using size.y=0 will size to
content in many layouts.
        // The Border flag gives us a tidy panel look.
        if (ImGui::BeginChild("vbox_child",
                              ImVec2(0.0f, 0.0f),
#if defined(ImGuiChildFlags_Border)
                              ImGuiChildFlags_Border |
ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysUseWindowPadding #else true
// Border (for older ImGui) – keep simple #endif
        )) {
            if (label) {
                DrawTitleLine_(label);
            }
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,
ImVec2(ImGui::GetStyle().ItemSpacing.x, ImGui::GetStyle().ItemSpacing.y *
0.6f)); DrawChildren(); ImGui::PopStyleVar();
        }
        ImGui::EndChild();
        ImGui::PopStyleVar();
    }
    */
};

DEFINE_WIDGET_FACTORY(VBoxWidget, VBox)

} // namespace augr
