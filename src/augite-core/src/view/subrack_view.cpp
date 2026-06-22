#include "imgui.h"
#include "imnodes.h"

#include <augr/rack/pin.h>
#include <augr/rack/rack.h>
#include <augr/rack/rack_doc.h>
#include <augr/rack/wire.h>

#include <augite/widget/module_widget.h>
#include <augite/widget/widget_builder.h>

#include <augite/view/subrack_view.h>

namespace augr {

SubrackView::SubrackView(RackDoc &doc) : DocumentViewT<RackDoc>(doc) {
    context_ = ImNodes::EditorContextCreate();
}

SubrackView::~SubrackView() {
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
    BuildControls();
}

void SubrackView::BuildControls() {
    ModelWidgetBuilder builder;
    controls_root_ = new Widget(); // dummy root to hold the real root's children
    AddChild(Widget::Ptr(controls_root_)); // take ownership of the dummy root
    builder.BuildChildren(*controls_root_, *subrack()->controls_);
}

void SubrackView::PopulateWidgetMap(Widget *widget) {
    if (!widget)
        return;

    if (widget != root_) {
        if (auto *mw = dynamic_cast<ModelWidget *>(widget)) {
            widget_map_[mw->model().id_] = mw;
        }
    }

    for (auto &child : widget->children_) {
        PopulateWidgetMap(child.get());
    }
}

void SubrackView::DrawControls() {
    controls_root_->Draw();
}

void SubrackView::Draw() {
    //if (ImGui::CollapsingHeader("Subrack View", ImGuiTreeNodeFlags_DefaultOpen)) {
    if (ImGui::CollapsingHeader("controls")) {
        DrawControls();
    }

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

/*
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
*/

} // namespace augr