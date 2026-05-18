#include "imgui.h"
#include "imnodes.h"

#include <augr/rack/pin.h>
#include <augr/rack/rack.h>
#include <augr/rack/rack_doc.h>
#include <augr/rack/wire.h>

#include <augite/widget/module_widget.h>

#include "rack_view.h"

namespace augr {

RackView::RackView(RackDoc &doc)
    : DocumentViewT<RackDoc>(doc), rack_(&doc.rack()) {}

RackView::~RackView() {
    if (root_) {
        delete root_;
        root_ = nullptr;
    }
}

void RackView::Build() {
    ModelView::Build();

    // After build completes, traverse root_ and populate widget_map_
    if (root_) {
        PopulateWidgetMap(root_);
    }
}

void RackView::PopulateWidgetMap(Widget *widget) {
    if (!widget)
        return;

    if (auto *mw = dynamic_cast<ModelWidget *>(widget)) {
        widget_map_[mw->model()->id_] = mw;
    }

    for (auto *child : widget->children_) {
        PopulateWidgetMap(child);
    }
}

void RackView::Draw() {
    if (root_ == nullptr) {
        Build();
    }

    ImNodes::BeginNodeEditor();

    root_->Draw();

    for (auto wire : rack_->wires_) {
        ImNodes::Link(wire->id_, wire->output_->id_, wire->input_->id_);
    }

    is_editor_hovered_ = ImNodes::IsEditorHovered();

    ImNodes::EndNodeEditor();
}

} // namespace augr