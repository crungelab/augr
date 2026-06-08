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

#include <augite/frame/subrack_viewer.h>
#include <augite/widget/module_widget.h>
#include <augite/widget/subrack_widget.h>
#include <augite/widget/widget_builder.h>

#include <augite/app/app.h>
#include <augite/inspector/inspector_dock.h>

#include "rack_selection.h"

namespace augr {

SubrackController::SubrackController(RackDoc &doc, SubrackView &view,
                                     Frame &frame)
    : DocumentController<RackDoc, SubrackView>(doc, view, frame) {}

// ----- Per-frame entry point -----

void SubrackController::Control() {
    CheckPendingSelection();
    CheckMouse();
    CheckKeyboard();
    CheckLinkCreated();
    CheckLinkDestroyed();
    CheckCreateNode();
    CheckNodeSelection();
    CheckInspection();

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
        out_models.push_back(&mw->model());
        out_widgets.push_back(mw);
    }
}

// ----- Actions -----

void SubrackController::SelectAll() {
    pending_selection_.clear();
    for (const auto &m : subrack().children_)
        pending_selection_.push_back(m->id_);
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
        document().MarkModified();
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
    document().MarkModified();
}

Vec2 SubrackController::ComputePasteOffset(
    const nlohmann::json &selection_json,
    std::optional<Vec2> anchor_grid_pos) const {

    if (!anchor_grid_pos.has_value())
        return Vec2{20.0f, 20.0f};

    Vec2 origin{0.0f, 0.0f};
    if (selection_json.contains("origin") &&
        selection_json["origin"].is_array() &&
        selection_json["origin"].size() == 2) {
        origin.x = selection_json["origin"][0].get<float>();
        origin.y = selection_json["origin"][1].get<float>();
    }
    return *anchor_grid_pos - origin;
}

void SubrackController::DeleteSelection() {
    if (!HasSelection())
        return;

    auto &wmap = view().widget_map();

    for (int node_id : selected_nodes_) {
        auto it = wmap.find(node_id);
        if (it == wmap.end())
            continue;
        Widget *widget = it->second;
        auto *mw = dynamic_cast<ModelWidget *>(widget);
        if (!mw)
            continue;

        if (auto *mod = dynamic_cast<Module *>(&mw->model()))
            //subrack().RemoveChild(*mod);
            mod->Destroy();
        wmap.erase(it);
        widget->Destroy();
    }
    selected_nodes_.clear();
    ImNodes::ClearNodeSelection();
    document().MarkModified();
}

Module *SubrackController::SpawnModule(const std::string &type_name,
                                       Vec2 grid_pos, int auto_connect_pin_id) {
    auto &mm = ModelManufacturer::singleton();
    ModelFactory *mf = mm.FindFactory(type_name);
    if (!mf) {
        std::cerr << "SpawnModule: unknown type '" << type_name << "'\n";
        return nullptr;
    }

    auto module_ptr = mf->Produce(subrack().shared_from_this());
    auto *module = dynamic_cast<Module *>(module_ptr.get());
    if (!module)
        return nullptr;

    ModelWidgetBuilder builder;
    auto widget = builder.Build(*module);
    auto *mw = dynamic_cast<ModuleWidget *>(widget.get());
    if (view().root_)
        view().root_->AddChild(std::move(widget));
    if (mw) {
        view().widget_map()[module->id_] = mw;
        mw->grid_position_ = grid_pos;
        mw->position_dirty_ = true;
    }

    if (auto_connect_pin_id != -1) {
        bool start_is_output = subrack().IsOutput(auto_connect_pin_id);
        Pin *output = nullptr;
        Pin *input = nullptr;
        if (start_is_output) {
            output = subrack().output_map_[auto_connect_pin_id];
            if (!module->inport_.pins_.empty())
                input = module->inport_.pins_[0];
        } else {
            if (!module->outport_.pins_.empty())
                output = module->outport_.pins_[0];
            input = subrack().input_map_[auto_connect_pin_id];
        }
        if (output && input)
            subrack().Connect(*output, *input);
    }

    document().MarkModified();
    return module;
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
                    auto *sr = &dynamic_cast<Subrack &>(mw->model());
                    auto *doc = dynamic_cast<RackDoc *>(doc_);
                    auto *child = new SubrackViewer(*doc, *sr, sr->label_);
                    child->Create(&frame());
                } else if (auto *mw =
                               dynamic_cast<ModuleWidget *>(it->second)) {
                    mw->is_open_ = !mw->is_open_;
                    mw->window_pose_dirty_ = true;
                }
            }
        }

        if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
            ImGui::OpenPopup("node_ctx");
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

    if (ImGui::IsKeyPressed(ImGuiKey_A, false))
        SelectAll();
    else if (ImGui::IsKeyPressed(ImGuiKey_C, false))
        Copy();
    else if (ImGui::IsKeyPressed(ImGuiKey_V, false))
        Paste(ScreenToGrid(ImGui::GetMousePos()));
    else if (ImGui::IsKeyPressed(ImGuiKey_X, false))
        Cut();
    else if (ImGui::IsKeyPressed(ImGuiKey_D, false))
        Duplicate();
}

void SubrackController::CheckLinkCreated() {
    int start_id, end_id;
    bool from_snap;
    if (ImNodes::IsLinkCreated(&start_id, &end_id, &from_snap)) {
        Pin *out = subrack().output_map_[start_id];
        Pin *in = subrack().input_map_[end_id];
        if (out && in) {
            subrack().Connect(*out, *in);
            document().MarkModified();
        }
    }
}

void SubrackController::CheckLinkDestroyed() {
    int link_id;
    if (ImNodes::IsLinkDestroyed(&link_id)) {
        auto it = subrack().wire_map_.find(link_id);
        if (it != subrack().wire_map_.end()) {
            subrack().Disconnect(*it->second);
            document().MarkModified();
        }
    }
}

void SubrackController::CheckCreateNode() {
    int dummy = -1;

    if (ImNodes::IsLinkDropped(&pending_link_start_attr_, false)) {
        pending_spawn_pos_ = ImGui::GetMousePos();
        catalog_popup_requested_ = true;
    }

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

    if (ImGui::MenuItem("Add Module..."))
        catalog_popup_requested_ = true;

    ImGui::Separator();

    if (ImGui::MenuItem("Paste Here", "Ctrl+V", false, has_clipboard))
        Paste(ScreenToGrid(pending_spawn_pos_));

    ImGui::Separator();

    if (ImGui::MenuItem("Select All", "Ctrl+A", false, has_anything))
        SelectAll();

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

    if (ImGui::IsKeyPressed(ImGuiKey_Escape, false)) {
        ImNodes::ClearNodeSelection();
        selected_nodes_.clear();
    } else if (ImGui::IsKeyPressed(ImGuiKey_Delete, false)) {
        DeleteSelection();
    }
}

void SubrackController::CheckInspection() {
    if (selected_nodes_.size() == 1) {
        auto &wmap = view().widget_map();
        auto it = wmap.find(selected_nodes_[0]);
        Module *target_module = nullptr;
        if (it != wmap.end()) {
            if (auto *tw = dynamic_cast<ModuleWidget *>(it->second))
                target_module = dynamic_cast<Module *>(&tw->model());
        }
        App::singleton().Inspect(target_module);
    } else {
        App::singleton().Inspect(nullptr);
    }
}

// ----- Popups -----

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

    const int target_id = hovered_node_id_;

    auto &wmap = view().widget_map();
    auto it = wmap.find(target_id);
    Module *target_module = nullptr;
    ModuleWidget *target_widget = nullptr;
    if (it != wmap.end()) {
        target_widget = dynamic_cast<ModuleWidget *>(it->second);
        if (target_widget)
            target_module = dynamic_cast<Module *>(&target_widget->model());
    }

    if (!target_module) {
        ImGui::TextDisabled("(node unavailable)");
        ImGui::EndPopup();
        return;
    }

    ImGui::TextDisabled("%s", target_module->label_.c_str());
    ImGui::Separator();

    const bool target_in_selection =
        std::find(selected_nodes_.begin(), selected_nodes_.end(), target_id) !=
        selected_nodes_.end();
    const bool acts_on_selection =
        target_in_selection && selected_nodes_.size() > 1;

    if (target_widget) {
        const char *label =
            target_widget->is_open_ ? "Close Window" : "Open Window";
        if (ImGui::MenuItem(label)) {
            target_widget->is_open_ = !target_widget->is_open_;
            target_widget->window_pose_dirty_ = true;
        }
    }

    ImGui::Separator();

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

    if (ImGui::MenuItem("Disconnect All Wires"))
        DisconnectAllWires(*target_module);

    ImGui::Separator();

    if (ImGui::MenuItem("Delete", "Del")) {
        if (!target_in_selection)
            SelectOnlyTarget(target_id);
        DeleteSelection();
    }

    if (acts_on_selection) {
        ImGui::Separator();
        ImGui::TextDisabled("(applies to %zu selected nodes)",
                            selected_nodes_.size());
    }

    ImGui::EndPopup();
}

// ----- Helpers -----

Vec2 SubrackController::ScreenToGrid(ImVec2 screen_pos) const {
    ImVec2 panning = ImNodes::EditorContextGetPanning();

    if (!subrack().children_.empty()) {
        if (auto *node =
                dynamic_cast<Node *>(subrack().children_.front().get())) {
            ImVec2 node_screen = ImNodes::GetNodeScreenSpacePos(node->id_);
            ImVec2 node_grid = ImNodes::GetNodeGridSpacePos(node->id_);
            return Vec2{node_grid.x + (screen_pos.x - node_screen.x),
                        node_grid.y + (screen_pos.y - node_screen.y)};
        }
    }

    return Vec2{screen_pos.x - panning.x, screen_pos.y - panning.y};
}

bool SubrackController::HasClipboardSelection() const {
    const char *text = ImGui::GetClipboardText();
    if (!text || !*text)
        return false;
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
    for (int id : pending_selection_)
        ImNodes::SelectNode(id);
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
    std::vector<Wire *> to_disconnect;
    auto collect = [&](const std::vector<Pin *> &pins) {
        for (Pin *pin : pins)
            for (Wire *w : pin->wires_)
                to_disconnect.push_back(w);
    };
    collect(target.inport_.pins_);
    collect(target.outport_.pins_);

    for (Wire *w : to_disconnect)
        subrack().Disconnect(*w);
    if (!to_disconnect.empty())
        document().MarkModified();
}

} // namespace augr