#pragma once

#include <map>

//#include "imgui.h"

#include "document_view.h"

class ImNodesEditorContext;

namespace augr {

class Subrack;
class RackDoc;

class SubrackView : public DocumentViewT<RackDoc> {
public:
    SubrackView(RackDoc &doc);
    ~SubrackView();

    void Build() override;
    void Draw() override;

    // Accessors
    Widget *root() { return root_; }
    const Widget *root() const { return root_; }

    Subrack *subrack() { return subrack_; }
    const Subrack *subrack() const { return subrack_; }

    std::map<int, Widget *> &widget_map() { return widget_map_; }
    const std::map<int, Widget *> &widget_map() const { return widget_map_; }

    // Accessors
    void set_subrack(Subrack &subrack) { subrack_ = &subrack; }
    bool is_editor_hovered() const { return is_editor_hovered_; }

private:
    void PopulateWidgetMap(Widget *widget);

    // Data members
    Subrack *subrack_ = nullptr; // = &doc_->rack(), cached for convenience
    std::map<int, Widget *> widget_map_;
    bool is_editor_hovered_ = false;
    ImNodesEditorContext* context_ = nullptr;

};

} // namespace augr