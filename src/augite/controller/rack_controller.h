#pragma once

#include <string>
#include <vector>

#include <imgui.h>

#include <augr/core/math/vec2.h>

#include "controller.h"

#include "../view/rack_view.h"

namespace augr {

class RackDoc;
class Rack;
class Model;
class Module;
class ModelWidget;

class RackController : public ControllerT<RackDoc, RackView> {
public:
    RackController(RackDoc &doc, RackView &view);

    // Polled once per frame from RackFrame::Draw, after view().Draw().
    void Control() override;

    // ---- Selection queries (used by menu enable/disable, etc.) ----

    bool HasSelection() const { return !selected_nodes_.empty(); }
    std::size_t SelectionSize() const { return selected_nodes_.size(); }

    // Resolves the current ImNodes selection into model/widget pointers.
    // Parallel ordering: models[i] corresponds to widgets[i]. Entries
    // that can't be resolved are skipped from both lists together.
    void CollectSelection(std::vector<Model *> &out_models,
                          std::vector<Widget *> &out_widgets) const;

    // ---- Actions (callable from menu items and Control loop) ----

    void Copy();
    void Paste();
    void Cut();
    void Duplicate();
    void DeleteSelection();

    // Construct a new module of the named type at the given grid
    // position. If `auto_connect_pin_id` is non-negative, attempts to
    // wire it to the first compatible pin on the new module (used by
    // drag-to-spawn). Returns the new module, or nullptr on failure.
    Module *SpawnModule(const std::string &type_name, Vec2 grid_pos,
                        int auto_connect_pin_id = -1);

private:
    // ---- Per-frame input polling (called from Control) ----

    void CheckMouse();
    void CheckLinkCreated();
    void CheckLinkDestroyed();
    void CheckCreateNode();
    void CheckNodeSelection();
    void CheckClipboard();

    // ---- Popup rendering (controller-owned in option 2) ----

    void DrawCatalogPopup();
    void DrawNodeContextMenu();

    // ---- Helpers ----

    // After a paste/duplicate, update ImNodes selection to highlight
    // the newly-created modules so the user can immediately drag them.
    //void SelectModelsInImNodes(const std::vector<Model *> &models);
    void SetPendingSelection(const std::vector<Model *> &models);
    void CheckPendingSelection();

    // Convenience accessors for the rack inside the document.
    Rack &rack();
    const Rack &rack() const;

    // ---- State ----

    std::vector<int> selected_nodes_;
    std::vector<int> pending_selection_;
    int hovered_node_id_ = -1;

    // Pending interactions waiting for the catalog popup to resolve.
    int pending_link_start_attr_ = -1;
    ImVec2 pending_spawn_pos_ = {0, 0};
    bool catalog_popup_requested_ = false;
};

} // namespace augr