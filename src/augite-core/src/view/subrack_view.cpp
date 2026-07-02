#include "imgui.h"
#include "imnodes.h"

#include <augr/rack/pin/pin.h>
#include <augr/rack/rack.h>
#include <augr/rack/rack_doc.h>
#include <augr/rack/wire.h>

#include <augite/widget/module_widget.h>

#include <augite/view/subrack_view.h>

namespace augr {

SubrackView::SubrackView(Subrack &subrack) : ViewT<Subrack>(subrack) {
    context_ = ImNodes::EditorContextCreate();
}

SubrackView::~SubrackView() {
    if (context_) {
        ImNodes::EditorContextFree(context_);
        context_ = nullptr;
    }
}

void SubrackView::Build() {
    ViewT<Subrack>::Build();

    PopulateWidgetMap(this);
}

void SubrackView::PopulateWidgetMap(Widget *widget) {
    if (!widget)
        return;

    if (widget != this) {
        if (auto *mw = dynamic_cast<ModelWidget *>(widget)) {
            widget_map_[mw->model().id_] = mw;
        }
    }

    for (auto &child : widget->children_) {
        PopulateWidgetMap(child.get());
    }
}

void SubrackView::Draw() {
    ImNodes::EditorContextSet(context_);

    ImNodes::BeginNodeEditor();

    View::Draw();

    for (auto wire : subrack()->wires_) {
        ImNodes::Link(wire->id_, wire->output_->id_, wire->input_->id_);
    }

    is_editor_hovered_ = ImNodes::IsEditorHovered();

    ImNodes::EndNodeEditor();
}

} // namespace augr