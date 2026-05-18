#include "imgui.h"

#include "subrack_widget.h"
#include <augr/rack/subrack.h>

namespace augr {

void SubrackWidget::DrawNodeContent() {
    ImGui::Dummy(ImVec2(0, 0));
    //ImGui::Text("Subrack");
}

DEFINE_MODEL_WIDGET_FACTORY(SubrackWidget, Subrack)

} // namespace augr