#include "imgui.h"

#include <augr/rack/subrack.h>

#include <augite/viewer/subrack_viewer.h>

#include "../archiver/module_widget_archiver.h"

#include "subrack_widget.h"

namespace augr {

void SubrackWidget::DrawNodeContent() {
    ModuleWidgetT<Subrack>::DrawNodeContent();
    ImGui::Dummy(ImVec2(0, 0));
    // ImGui::Text("Subrack");
}

void SubrackWidget::OnLeftDoubleClick(RackDoc &doc, Frame &parent_frame) {
    ModuleWidgetT<Subrack>::OnLeftDoubleClick(doc, parent_frame);
    if (is_open_ && !viewer_) {
        auto &sr = model();
        viewer_ = new SubrackViewer(doc, sr, sr.label_);
        viewer_->Create(&parent_frame);
    } else if (!is_open_ && viewer_) {
        viewer_->Destroy();
        viewer_ = nullptr;
    }
}

DEFINE_MODEL_WIDGET_FACTORY(SubrackWidget, Subrack)

class SubrackWidgetArchiver : public ModuleWidgetArchiver {};
DEFINE_ARCHIVER_FACTORY(SubrackWidgetArchiver, SubrackWidget, "Widget.Subrack")

} // namespace augr