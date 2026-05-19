#include "imgui.h"
#include "imnodes.h"

#include <augr/rack/pin.h>
#include <augr/rack/rack.h>
#include <augr/rack/rack_doc.h>
#include <augr/rack/wire.h>

#include <augite/widget/module_widget.h>

#include "subrack_view.h"

namespace augr {

SubrackView::SubrackView(RackDoc &doc)
    : DocumentViewT<RackDoc>(doc) {
        context_ = ImNodes::EditorContextCreate();
    }

SubrackView::~SubrackView() {
    if (root_) {
        delete root_;
        root_ = nullptr;
    }
    if (context_) {
        ImNodes::EditorContextFree(context_);
        context_ = nullptr;
    }
}

void SubrackView::Build() {
    ModelView::Build();

    // After build completes, traverse root_ and populate widget_map_
    if (root_) {
        PopulateWidgetMap(root_);
    }
}

void SubrackView::PopulateWidgetMap(Widget *widget) {
    if (!widget)
        return;

    if (widget == root_)
        return; // root_ is a dummy top-level widget, not in the map

    if (auto *mw = dynamic_cast<ModelWidget *>(widget)) {
        widget_map_[mw->model()->id_] = mw;
    }

    for (auto *child : widget->children_) {
        PopulateWidgetMap(child);
    }
}

void SubrackView::Draw() {
    ImNodes::EditorContextSet(context_);

    if (root_ == nullptr) {
        Build();
    }

    ImNodes::BeginNodeEditor();

    root_->Draw();

    for (auto wire : subrack()->wires_) {
        ImNodes::Link(wire->id_, wire->output_->id_, wire->input_->id_);
    }

    is_editor_hovered_ = ImNodes::IsEditorHovered();

    ImNodes::EndNodeEditor();
}

} // namespace augr