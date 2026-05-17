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
    void PopulateWidgetMap(Widget *widget);

    void Draw() override;

    void DrawModuleCatalog();

    void CheckLinkCreated();
    void CheckLinkDestroyed();
    void CheckCreateNode();
    void CheckNodeSelection();
    void CheckMouse();
    void CheckClipboard();

    void HandleCopy();
    void HandlePaste();
    void HandleCut();
    void HandleDuplicate();

    std::vector<Model*> SelectedModules() const;
    std::vector<Widget*> SelectedWidgets() const;

    // Accessors
    std::map<int, Widget *>& widget_map() { return widget_map_; }

private:
    // Data members
    Rack *rack_ = nullptr; // = &doc_->rack(), cached for convenience
    std::map<int, Widget *> widget_map_;
    bool is_editor_hovered_ = false;
    int hovered_node_id = -1;
    int pending_link_start_attr = -1;
    ImVec2 pending_spawn_pos = ImVec2(0, 0);
    std::vector<int> selected_nodes_;
};

} // namespace augr