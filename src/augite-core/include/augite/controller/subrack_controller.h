#pragma once

#include <string>
#include <vector>

#include <imgui.h>
#include <nlohmann/json.hpp>

#include <augr/math/vec2.h>

#include "controller.h"

#include "../view/subrack_view.h"

namespace augr {

class RackDoc;
class Subrack;
class Model;
class Module;
class ModelWidget;
class Frame;

class SubrackController : public ControllerT<RackDoc, Subrack, SubrackView> {
public:
    SubrackController(RackDoc &doc, Subrack &subrack, SubrackView &view, Frame &frame);

    // Polled once per frame from RackViewer::Draw, after view().Draw().
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

    void SelectAll();
    void Copy();
    void Cut();
    void Paste(std::optional<Vec2> anchor_grid_pos = std::nullopt);
    void Duplicate(std::optional<Vec2> anchor_grid_pos = std::nullopt);

    void DeleteSelection();
    bool HasClipboardSelection() const;

    // Construct a new module of the named type at the given grid
    // position. If `auto_connect_pin_id` is non-negative, attempts to
    // wire it to the first compatible pin on the new module (used by
    // drag-to-spawn). Returns the new module, or nullptr on failure.
    Module *SpawnModule(const std::string &type_name, Vec2 grid_pos,
                        int auto_connect_pin_id = -1);

    // Accessors
    Subrack &subrack() { return *static_cast<Subrack *>(model_); }
    const Subrack &subrack() const {
        return *static_cast<const Subrack *>(model_);
    }

private:
    // ---- Per-frame input polling (called from Control) ----

    void CheckMouse();
    void CheckKeyboard();
    void CheckLinkCreated();
    void CheckLinkDestroyed();
    void CheckCreateNode();
    void CheckNodeSelection();
    void CheckInspection();

    // ---- Popup rendering (controller-owned in option 2) ----

    void DrawCatalogPopup();
    void DrawNodeContextMenu();
    void DrawGridContextMenu();

    // ---- Helpers ----
    Vec2 ScreenToGrid(ImVec2 screen_pos) const;
    Vec2 ComputePasteOffset(
        const nlohmann::json &selection_json,
        std::optional<Vec2> anchor_grid_pos = std::nullopt) const;
    // After a paste/duplicate, update ImNodes selection to highlight
    // the newly-created modules so the user can immediately drag them.
    void SetPendingSelection(const std::vector<Model *> &models);
    void CheckPendingSelection();

    void SelectOnlyTarget(int node_id);
    void DisconnectAllWires(Module &target);

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