#include "imgui.h"

#include <augr/rack/subrack.h>

#include <augite/viewer/subrack_viewer.h>
#include <augite/app/app.h>

#include <augite/archiver/module_widget_archiver.h>

#include <augite/widget/subrack_widget.h>

namespace augr {

void SubrackWidget::DrawNodeContent() {
    ModuleWidgetT<Subrack>::DrawNodeContent();
    ImGui::Dummy(ImVec2(0, 0));
    // ImGui::Text("Subrack");
}

DEFINE_MODEL_WIDGET_FACTORY(SubrackWidget, Subrack)

class SubrackWidgetArchiver : public ModuleWidgetArchiver {};
DEFINE_ARCHIVER_FACTORY(SubrackWidgetArchiver, SubrackWidget, "SubrackWidget")

} // namespace augr