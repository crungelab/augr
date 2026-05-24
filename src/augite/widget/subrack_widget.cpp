#include "imgui.h"

#include <augr/rack/subrack.h>

#include "../archiver/module_widget_archiver.h"

#include "subrack_widget.h"



namespace augr {

void SubrackWidget::DrawNodeContent() {
    ImGui::Dummy(ImVec2(0, 0));
    //ImGui::Text("Subrack");
}

DEFINE_MODEL_WIDGET_FACTORY(SubrackWidget, Subrack)

class SubrackWidgetArchiver : public ModuleWidgetArchiver {};
DEFINE_ARCHIVER_FACTORY(SubrackWidgetArchiver, SubrackWidget, "Widget.Subrack")

} // namespace augr