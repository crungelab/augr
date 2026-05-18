#pragma once

#include <map>

#include "imgui.h"

#include "document_view.h"

namespace augr {

class Rack;
class RackDoc;

class RackView : public DocumentViewT<RackDoc> {
public:
    RackView(RackDoc &doc);
    ~RackView();

    void Build() override;
    void Draw() override;

    // Accessors
    Widget *root() { return root_; }
    const Widget *root() const { return root_; }

    Rack *rack() { return rack_; }
    const Rack *rack() const { return rack_; }

    std::map<int, Widget *> &widget_map() { return widget_map_; }
    const std::map<int, Widget *> &widget_map() const { return widget_map_; }

    bool is_editor_hovered() const { return is_editor_hovered_; }

private:
    void PopulateWidgetMap(Widget *widget);

    // Data members
    Rack *rack_ = nullptr; // = &doc_->rack(), cached for convenience
    std::map<int, Widget *> widget_map_;
    bool is_editor_hovered_ = false;

};

} // namespace augr