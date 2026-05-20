#include "subrack_frame.h"

#include <imgui.h>

namespace augr {

SubrackFrame::SubrackFrame(RackDoc &doc, Subrack &subrack,
                           const std::string &label)
    : FrameT<RackDoc, SubrackView, SubrackController>(doc, label),
      subrack_(&subrack) {

    RebuildView();
}

SubrackFrame::~SubrackFrame() = default;

void SubrackFrame::RebuildView() {
    view_ = std::make_unique<SubrackView>(doc());
    view().set_subrack(subrack());
    view().Build(); // construct widget tree now so view archiver has something
                    // to load into
    controller_ = std::make_unique<SubrackController>(doc(), view(), *this);
    controller().set_subrack(subrack());

}

void SubrackFrame::Begin() {
    // Use a unique ImGui window ID per subrack so multiple frames
    // (different subracks) don't collide. The subrack pointer is stable
    // for the frame's lifetime.
    char title[128];
    std::snprintf(title, sizeof(title), "%s###subrack_%p",
                  label_.empty() ? "Subrack" : label_.c_str(),
                  static_cast<void *>(subrack_));
    bool p_open = true;
    ImGui::Begin(title, &p_open, ImGuiWindowFlags_NoCollapse);
    if (!p_open) {
        // User closed the window; close the frame.
        parent_->RemoveChild(*this);
    }
}

/*
void SubrackFrame::Draw() {
    Begin();

    // Standard MVC draw cycle: view renders the graph editor, then the
    // controller polls input against what was just rendered.
    view().Draw();
    controller().Control();

    ImGui::End();
}
*/

} // namespace augr