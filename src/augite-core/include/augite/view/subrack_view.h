#pragma once

#include <map>

#include "view.h"
#include "augr/rack/subrack.h"

class ImNodesEditorContext;

namespace augr {

class RackDoc;

class SubrackView : public ViewT<Subrack> {
public:
    explicit SubrackView(Subrack &subrack);
    ~SubrackView();

    void Build() override;

    void Draw() override;

    // Accessors
    Subrack *subrack() { return static_cast<Subrack *>(model_); }
    const Subrack *subrack() const { return static_cast<const Subrack *>(model_); }

    Widget *root() { return root_; }
    const Widget *root() const { return root_; }

    std::map<int, Widget *> &widget_map() { return widget_map_; }
    const std::map<int, Widget *> &widget_map() const { return widget_map_; }

    bool is_editor_hovered() const { return is_editor_hovered_; }

private:
    void PopulateWidgetMap(Widget *widget);

    // Data members
    std::map<int, Widget *> widget_map_;
    bool is_editor_hovered_ = false;
    ImNodesEditorContext *context_ = nullptr;
};

} // namespace augr