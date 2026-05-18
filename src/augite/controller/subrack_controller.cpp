// subrack_controller.cpp
#include "subrack_controller.h"

#include <iostream>

#include <imgui.h>
#include <imnodes.h>
#include <nlohmann/json.hpp>

#include <augr/core/model_manufacturer.h>
#include <augr/rack/module/module.h>
#include <augr/rack/pin.h>
#include <augr/rack/rack_doc.h>
#include <augr/rack/subrack.h>
#include <augr/rack/wire.h>

#include <augite/frame/subrack_frame.h>
#include <augite/widget/module_widget.h>
#include <augite/widget/subrack_widget.h>
#include <augite/widget/widget_builder.h>

#include "rack_selection.h"

namespace augr {

SubrackController::SubrackController(RackDoc &doc, SubrackView &view,
                                     Frame &frame)
    : ControllerT<RackDoc, SubrackView>(doc, view, frame) {}

Subrack &SubrackController::subrack() {
    return subrack_ ? *subrack_ : static_cast<Subrack &>(doc().rack());
}

const Subrack &SubrackController::subrack() const {
    return subrack_ ? *subrack_ : static_cast<const Subrack &>(doc().rack());
}

/*
SubrackController::SubrackController(RackDoc &doc, SubrackView &view)
    : ControllerT<RackDoc, SubrackView>(doc, view) {}

Subrack &SubrackController::subrack() { return doc().rack(); }
const Subrack &SubrackController::subrack() const { return doc().rack(); }
*/

// ----- Per-frame entry point -----

void SubrackController::Control() {
    CheckPendingSelection();
    CheckMouse();
    CheckKeyboard();
    CheckLinkCreated();
    CheckLinkDestroyed();
    CheckCreateNode();
    CheckNodeSelection();
    DrawCatalogPopup();
    DrawNodeContextMenu();
    DrawGridContextMenu();
}

// ----- Selection -----

void SubrackController::CollectSelection(
    std::vector<Model *> &out_models,
    std::vector<Widget *> &out_widgets) const {
    out_models.clear();
    out_widgets.clear();
    const auto &wmap = view().widget_map();
    for (int id : selected_nodes_) {
        auto it = wmap.find(id);
        if (it == wmap.end())
            continue;
        auto *mw = dynamic_cast<ModelWidget *>(it->second);
        if (!mw)
            continue;
        Model *m = mw->model();
        if (!m)
            continue;
        out_models.push_back(m);
        out_widgets.push_back(mw);
    }
}

// ----- Actions -----

void SubrackController::SelectAll() {
    pending_selection_.clear();
    for (Model *m : subrack().children_) {
        pending_selection_.push_back(m->id_);
    }
}

void SubrackController::Copy() {
    std::vector<Model *> models;
    std::vector<Widget *> widgets;
    CollectSelection(models, widgets);
    if (models.empty())
        return;

    nlohmann::json j =
        RackSelection::BuildSelectionJson(subrack(), view(), models, widgets);
    ImGui::SetClipboardText(j.dump().c_str());
}

void SubrackController::Cut() {
    if (!HasSelection())
        return;
    Copy();
    DeleteSelection();
}

// In rack_controller.cpp:
void SubrackController::Paste(std::optional<Vec2> anchor_grid_pos) {
    const char *text = ImGui::GetClipboardText();
    if (!text)
        return;
    try {
        nlohmann::json j = nlohmann::json::parse(text);
        if (!RackSelection::LooksLikeSelection(j))
            return;

        Vec2 offset = ComputePasteOffset(j, anchor_grid_pos);
        auto pasted =
            RackSelection::MergeIntoRack(subrack(), view(), j, offset);

        SetPendingSelection(pasted);
        doc().MarkModified();
    } catch (const std::exception &e) {
        std::cerr << "Paste failed: " << e.what() << "\n";
    }
}

void SubrackController::Duplicate(std::optional<Vec2> anchor_grid_pos) {
    if (!HasSelection())
        return;

    std::vector<Model *> models;
    std::vector<Widget *> widgets;
    CollectSelection(models, widgets);

    nlohmann::json j =
        RackSelection::BuildSelectionJson(subrack(), view(), models, widgets);

    Vec2 offset = ComputePasteOffset(j, anchor_grid_pos);
    auto pasted = RackSelection::MergeIntoRack(subrack(), view(), j, offset);

    SetPendingSelection(pasted);
    doc().MarkModified();
}

Vec2 SubrackController::ComputePasteOffset(
    const nlohmann::json &selection_json,
    std::optional<Vec2> anchor_grid_pos) const {

    // No anchor → fixed nudge (paste-near-source default behavior).
    if (!anchor_grid_pos.has_value()) {
        return Vec2{20.0f, 20.0f};
    }

    // Anchor specified — read origin from the clipboard JSON and
    // compute the delta so the bbox-min lands at the anchor.
    Vec2 origin{0.0f, 0.0f};
    if (selection_json.contains("origin") &&
        selection_json["origin"].is_array() &&
        selection_json["origin"].size() == 2) {
        origin.x = selection_json["origin"][0].get<float>();
        origin.y = selection_json["origin"][1].get<float>();
    }
    // If "origin" is missing (older clipboard payload, e.g.), the anchor
    // becomes the absolute paste position — that's a reasonable
    // fallback. Pasted nodes will land near the cursor but their internal
    // layout will start from (0,0) since they think they were copied
    // from there.
    return *anchor_grid_pos - origin;
}

void SubrackController::DeleteSelection() {
    if (!HasSelection())
        return;

    auto &wmap = view().widget_map();
    Widget *root = view().root_;

    for (int node_id : selected_nodes_) {
        auto it = wmap.find(node_id);
        if (it == wmap.end())
            continue;
        Widget *widget = it->second;
        auto *mw = dynamic_cast<ModelWidget *>(widget);
        if (!mw)
            continue;

        if (auto *mod = dynamic_cast<Module *>(mw->model())) {
            subrack().RemoveChild(*mod);
        }
        if (root)
            root->RemoveChild(*widget);
        wmap.erase(it);
        delete widget;
    }
    selected_nodes_.clear();
    ImNodes::ClearNodeSelection();
    doc().MarkModified();
}

Module *SubrackController::SpawnModule(const std::string &type_name,
                                       Vec2 grid_pos, int auto_connect_pin_id) {
    auto &mm = ModelManufacturer::singleton();
    ModelFactory *mf = mm.FindFactory(type_name);
    if (!mf) {
        std::cerr << "SpawnModule: unknown type '" << type_name << "'\n";
        return nullptr;
    }

    Module &module = dynamic_cast<Module &>(*mf->Produce(&subrack()));

    ModelWidgetBuilder builder;
    Widget *widget = builder.Build(module);
    if (view().root_) {
        view().root_->AddChild(widget);
    }
    if (auto *mw = dynamic_cast<ModuleWidget *>(widget)) {
        view().widget_map()[module.id_] = mw;
        mw->grid_position_ = grid_pos;
        mw->position_dirty_ = true;
    }

    // Auto-connect from a dropped link, if requested.
    if (auto_connect_pin_id != -1) {
        bool start_is_output = subrack().IsOutput(auto_connect_pin_id);
        Pin *output = nullptr;
        Pin *input = nullptr;
        if (start_is_output) {
            output = subrack().output_map_[auto_connect_pin_id];
            if (!module.inport_.pins_.empty()) {
                input = module.inport_.pins_[0];
            }
        } else {
            if (!module.outport_.pins_.empty()) {
                output = module.outport_.pins_[0];
            }
            input = subrack().input_map_[auto_connect_pin_id];
        }
        if (output && input) {
            subrack().Connect(*output, *input);
        }
    }

    doc().MarkModified();
    return &module;
}

// ----- Input polling -----

void SubrackController::CheckMouse() {
    int node_id = -1;
    if (ImNodes::IsNodeHovered(&node_id)) {
        hovered_node_id_ = node_id;

        if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
            const auto &wmap = view().widget_map();
            if (auto it = wmap.find(hovered_node_id_); it != wmap.end()) {
                if (auto *mw = dynamic_cast<SubrackWidget *>(it->second)) {
                    auto *subrack = &dynamic_cast<Subrack &>(*mw->model());
                    auto *doc = dynamic_cast<RackDoc *>(doc_);
                    auto *child =
                        new SubrackFrame(*doc, *subrack, subrack->label_);
                    this->frame().AddChild(child);
                } else if (auto *mw =
                               dynamic_cast<ModuleWidget *>(it->second)) {
                    mw->is_open_ = !mw->is_open_;
                    mw->window_pose_dirty_ = true;
                }
            }
        }

        if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
            ImGui::OpenPopup("node_ctx");
        }
    }
}

void SubrackController::CheckKeyboard() {
    if (!view().is_editor_hovered() || ImGui::GetIO().WantTextInput)
        return;

    if (ImGui::IsKeyPressed(ImGuiKey_Space, false)) {
        pending_link_start_attr_ = -1;
        pending_spawn_pos_ = ImGui::GetMousePos();
        catalog_popup_requested_ = true;
    }

    const bool ctrl = ImGui::GetIO().KeyCtrl;
    if (!ctrl)
        return;

    if (ImGui::IsKeyPressed(ImGuiKey_A, false)) {
        SelectAll();
    } else if (ImGui::IsKeyPressed(ImGuiKey_C, false)) {
        Copy();
    } else if (ImGui::IsKeyPressed(ImGuiKey_V, false)) {
        Paste(ScreenToGrid(ImGui::GetMousePos()));
    } else if (ImGui::IsKeyPressed(ImGuiKey_X, false)) {
        Cut();
    } else if (ImGui::IsKeyPressed(ImGuiKey_D, false)) {
        Duplicate();
    }
}

void SubrackController::CheckLinkCreated() {
    int start_id, end_id;
    bool from_snap;
    if (ImNodes::IsLinkCreated(&start_id, &end_id, &from_snap)) {
        Pin *out = subrack().output_map_[start_id];
        Pin *in = subrack().input_map_[end_id];
        if (out && in) {
            subrack().Connect(*out, *in);
            doc().MarkModified();
        }
    }
}

void SubrackController::CheckLinkDestroyed() {
    int link_id;
    if (ImNodes::IsLinkDestroyed(&link_id)) {
        auto it = subrack().wire_map_.find(link_id);
        if (it != subrack().wire_map_.end()) {
            subrack().Disconnect(*it->second);
            doc().MarkModified();
        }
    }
}

void SubrackController::CheckCreateNode() {
    int dummy = -1;

    // Link dropped into empty space → directly open catalog at cursor.
    // This stays a one-step interaction because the user's already
    // mid-gesture (dragging a link).
    if (ImNodes::IsLinkDropped(&pending_link_start_attr_,
                               /*including_detach=*/false)) {
        pending_spawn_pos_ = ImGui::GetMousePos();
        catalog_popup_requested_ = true;
    }

    // Right-click on empty grid → open the grid context menu, not the
    // catalog directly. The menu has an "Add Module..." item that
    // triggers the catalog from there.
    if (ImGui::IsMouseReleased(ImGuiMouseButton_Right) &&
        view().is_editor_hovered() && !ImNodes::IsAnyAttributeActive() &&
        !ImGui::IsAnyItemHovered() && !ImNodes::IsNodeHovered(&dummy)) {
        pending_link_start_attr_ = -1;
        pending_spawn_pos_ = ImGui::GetMousePos();
        ImGui::OpenPopup("grid_ctx");
    }
}

void SubrackController::DrawGridContextMenu() {
    if (!ImGui::BeginPopup("grid_ctx"))
        return;

    const bool has_clipboard = HasClipboardSelection();
    const bool has_anything = !subrack().children_.empty();

    if (ImGui::MenuItem("Add Module...")) {
        catalog_popup_requested_ = true;
    }

    ImGui::Separator();

    if (ImGui::MenuItem("Paste Here", "Ctrl+V", false, has_clipboard)) {
        Paste(ScreenToGrid(pending_spawn_pos_));
    }

    ImGui::Separator();

    if (ImGui::MenuItem("Select All", "Ctrl+A", false, has_anything)) {
        SelectAll();
    }

    ImGui::EndPopup();
}

void SubrackController::CheckNodeSelection() {
    const int num = ImNodes::NumSelectedNodes();
    if (num > 0) {
        selected_nodes_.resize(num);
        ImNodes::GetSelectedNodes(selected_nodes_.data());
    } else {
        selected_nodes_.clear();
    }

    if (ImGui::IsKeyPressed(ImGuiKey_Escape, /*repeat=*/false)) {
        ImNodes::ClearNodeSelection();
        selected_nodes_.clear();
    } else if (ImGui::IsKeyPressed(ImGuiKey_Delete, /*repeat=*/false)) {
        DeleteSelection();
    }
}

// ----- Popups (controller-owned) -----

void SubrackController::DrawCatalogPopup() {
    if (catalog_popup_requested_) {
        ImGui::OpenPopup("ModuleCatalog");
        catalog_popup_requested_ = false;
    }

    if (!ImGui::BeginPopup("ModuleCatalog"))
        return;

    ImGui::SetWindowPos(pending_spawn_pos_, ImGuiCond_Appearing);

    static char filter[64] = "";
    ImGui::InputTextWithHint("##filter", "Search modules…", filter,
                             IM_ARRAYSIZE(filter));

    auto &mm = ModelManufacturer::singleton();
    for (const auto &f : mm.factories_) {
        if (f->category_.empty())
            continue;
        if (filter[0] && !strstr(f->name_.c_str(), filter))
            continue;

        if (ImGui::Selectable(f->name_.c_str())) {
            Vec2 grid_pos = ScreenToGrid(pending_spawn_pos_);
            SpawnModule(f->name_, grid_pos, pending_link_start_attr_);
            pending_link_start_attr_ = -1;
            ImGui::CloseCurrentPopup();
        }
    }

    ImGui::EndPopup();
}

void SubrackController::DrawNodeContextMenu() {
    if (!ImGui::BeginPopup("node_ctx"))
        return;

    // hovered_node_id_ was captured at the moment of right-click in
    // CheckMouse. The popup outlives the hover state, so we keep using
    // the captured id throughout the popup's lifetime.
    const int target_id = hovered_node_id_;

    // Resolve target widget/module for the action labels.
    auto &wmap = view().widget_map();
    auto it = wmap.find(target_id);
    Module *target_module = nullptr;
    ModuleWidget *target_widget = nullptr;
    if (it != wmap.end()) {
        target_widget = dynamic_cast<ModuleWidget *>(it->second);
        if (target_widget) {
            target_module = dynamic_cast<Module *>(target_widget->model());
        }
    }

    if (!target_module) {
        // Stale id — node was deleted while popup is open, or never
        // resolvable. Close the popup gracefully.
        ImGui::TextDisabled("(node unavailable)");
        ImGui::EndPopup();
        return;
    }

    // Header — non-interactive label identifying the target.
    ImGui::TextDisabled("%s", target_module->label_.c_str());
    ImGui::Separator();

    // Whether the right-clicked node is part of a multi-selection.
    // Most operations should act on the whole selection if so, otherwise
    // on just the target.
    const bool target_in_selection =
        std::find(selected_nodes_.begin(), selected_nodes_.end(), target_id) !=
        selected_nodes_.end();
    const bool acts_on_selection =
        target_in_selection && selected_nodes_.size() > 1;

    // ----- Window visibility -----
    if (target_widget) {
        const char *label =
            target_widget->is_open_ ? "Close Window" : "Open Window";
        if (ImGui::MenuItem(label)) {
            target_widget->is_open_ = !target_widget->is_open_;
            target_widget->window_pose_dirty_ = true;
        }
    }

    ImGui::Separator();

    // ----- Standard edit actions -----
    // If the target is part of the current selection, these operate on
    // the whole selection. Otherwise, they implicitly act on just the
    // target by treating it as a one-element selection.
    if (ImGui::MenuItem("Cut", "Ctrl+X")) {
        if (!target_in_selection)
            SelectOnlyTarget(target_id);
        Cut();
    }
    if (ImGui::MenuItem("Copy", "Ctrl+C")) {
        if (!target_in_selection)
            SelectOnlyTarget(target_id);
        Copy();
    }
    if (ImGui::MenuItem("Duplicate", "Ctrl+D")) {
        if (!target_in_selection)
            SelectOnlyTarget(target_id);
        Duplicate();
    }

    ImGui::Separator();

    // ----- Disconnect -----
    if (ImGui::MenuItem("Disconnect All Wires")) {
        DisconnectAllWires(*target_module);
    }

    ImGui::Separator();

    // ----- Delete -----
    if (ImGui::MenuItem("Delete", "Del")) {
        if (!target_in_selection)
            SelectOnlyTarget(target_id);
        DeleteSelection();
    }

    // If multi-selection, hint at it.
    if (acts_on_selection) {
        ImGui::Separator();
        ImGui::TextDisabled("(applies to %zu selected nodes)",
                            selected_nodes_.size());
    }

    ImGui::EndPopup();
}

// ----- Helpers -----
Vec2 SubrackController::ScreenToGrid(ImVec2 screen_pos) const {
    // Convert screen → grid using ImNodes' panning state.
    // Editor space is screen space relative to the editor's top-left,
    // grid space is editor space minus the pan offset.
    //
    // ImNodes provides GetNodeEditorSpacePos for nodes; we approximate
    // for an arbitrary screen point.
    ImVec2 panning = ImNodes::EditorContextGetPanning();

    // We need to know where the editor begins on screen. The trick:
    // any existing node gives us a screen↔grid reference point.
    // Failing that, we fall back to assuming the editor starts at the
    // host window's content min.
    //
    // Simplest reliable approach: pick the first node we have, get both
    // its screen pos and grid pos, and use the delta to translate.
    if (!subrack().children_.empty()) {
        if (auto *node = dynamic_cast<Node *>(subrack().children_.front())) {
            ImVec2 node_screen = ImNodes::GetNodeScreenSpacePos(node->id_);
            ImVec2 node_grid = ImNodes::GetNodeGridSpacePos(node->id_);
            return Vec2{node_grid.x + (screen_pos.x - node_screen.x),
                        node_grid.y + (screen_pos.y - node_screen.y)};
        }
    }

    // Fallback when there are no nodes to use as a reference point.
    // Editor presumably starts at (0,0) with the current panning applied.
    return Vec2{screen_pos.x - panning.x, screen_pos.y - panning.y};
}

bool SubrackController::HasClipboardSelection() const {
    const char *text = ImGui::GetClipboardText();
    if (!text || !*text)
        return false;
    // Cheap structural sniff — don't fully parse the JSON every frame.
    // RackSelection has a string-overload version of LooksLikeSelection
    // if you've added one; otherwise this inline check is fine.
    return std::string_view(text).find("\"augr.selection\"") !=
           std::string_view::npos;
}

void SubrackController::SetPendingSelection(
    const std::vector<Model *> &models) {
    pending_selection_.clear();
    for (Model *m : models) {
        if (m)
            pending_selection_.push_back(m->id_);
    }
}

void SubrackController::CheckPendingSelection() {
    if (pending_selection_.empty())
        return;

    ImNodes::ClearNodeSelection();
    for (int id : pending_selection_) {
        ImNodes::SelectNode(id);
    }
    selected_nodes_ = pending_selection_;
    pending_selection_.clear();
}

void SubrackController::SelectOnlyTarget(int node_id) {
    ImNodes::ClearNodeSelection();
    ImNodes::SelectNode(node_id);
    selected_nodes_.clear();
    selected_nodes_.push_back(node_id);
}

void SubrackController::DisconnectAllWires(Module &target) {
    // Collect wires first to avoid iterator invalidation while Disconnecting.
    std::vector<Wire *> to_disconnect;
    auto collect = [&](const std::vector<Pin *> &pins) {
        for (Pin *pin : pins) {
            for (Wire *w : pin->wires_)
                to_disconnect.push_back(w);
        }
    };
    collect(target.inport_.pins_);
    collect(target.outport_.pins_);

    for (Wire *w : to_disconnect) {
        subrack().Disconnect(*w);
    }
    if (!to_disconnect.empty())
        doc().MarkModified();
}

} // namespace augr