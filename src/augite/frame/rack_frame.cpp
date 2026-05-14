#include <augr/rack/rack_doc.h>

#include "rack_frame.h"

namespace augr {

RackFrame::RackFrame(const std::string &label)
    : FrameT<RackDoc, RackView>(label) {
    doc_ = std::make_unique<RackDoc>();
    doc().NewDocument();
    RebuildView();
}

void RackFrame::RebuildView() { view_ = std::make_unique<RackView>(doc()); }

void RackFrame::Draw() {
    PollPendingDialog();
    DrawMenuBar();
    DrawUnsavedModal();
    Frame::Draw();
}

void RackFrame::Begin() {
    ImGuiWindowFlags graph_flags =
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoBringToFrontOnFocus; // <-- key

    ImGui::Begin(label_.c_str(), nullptr, graph_flags);
}

// ---------- Dialog polling ----------

void RackFrame::PollPendingDialog() {
    if (open_dialog_ && open_dialog_->ready(0)) {
        auto result = open_dialog_->result();
        open_dialog_.reset();
        if (!result.empty()) {
            DoOpen(result.front());
        }
    }
    if (save_dialog_ && save_dialog_->ready(0)) {
        auto result = save_dialog_->result();
        save_dialog_.reset();
        if (!result.empty()) {
            std::filesystem::path p = result;
            if (p.extension().empty())
                p.replace_extension(".augr");
            DoSaveAs(p);

            // If a save was the prelude to another action, fire it now.
            if (pending_ == PendingAction::OpenAfterPrompt) {
                StartOpenDialog();
            } else if (pending_ == PendingAction::NewAfterPrompt) {
                DoNew();
            }
            pending_ = PendingAction::None;
        } else {
            // User cancelled save — also cancel any pending follow-up.
            pending_ = PendingAction::None;
        }
    }
}

void RackFrame::DrawMenuBar() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New", "Ctrl+N")) {
                if (doc().IsModified()) {
                    pending_ = PendingAction::NewAfterPrompt;
                    show_unsaved_modal_ = true;
                } else {
                    DoNew();
                }
            }
            if (ImGui::MenuItem("Open...", "Ctrl+O")) {
                if (doc().IsModified()) {
                    pending_ = PendingAction::OpenAfterPrompt;
                    show_unsaved_modal_ = true;
                } else {
                    StartOpenDialog();
                }
            }
            ImGui::Separator();
            bool can_save = doc().IsModified() || !doc().Path();
            if (ImGui::MenuItem("Save", "Ctrl+S", false, can_save)) {
                if (doc().Path()) {
                    DoSave();
                } else {
                    StartSaveAsDialog();
                }
            }
            if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S")) {
                StartSaveAsDialog();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Quit", "Ctrl+Q")) {
                // similar dirty-check pattern; left as exercise
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

// ---------- Dialog launchers ----------

void RackFrame::StartOpenDialog() {
    std::string default_dir =
        doc().Path() ? doc().Path()->parent_path().string() : pfd::path::home();

    open_dialog_ = std::make_unique<pfd::open_file>(
        "Open Rack", default_dir,
        std::vector<std::string>{"Augr Rack (*.augr)", "*.augr", "All Files",
                                 "*"},
        pfd::opt::none);
}

void RackFrame::StartSaveAsDialog() {
    std::string default_path =
        doc().Path() ? doc().Path()->string() : "untitled.augr";

    save_dialog_ = std::make_unique<pfd::save_file>(
        "Save Rack As", default_path,
        std::vector<std::string>{"Augr Rack (*.augr)", "*.augr"},
        pfd::opt::force_overwrite);
}

// ---------- Document operations ----------

void RackFrame::DoNew() {
    doc().NewDocument();
    RebuildView();
}

void RackFrame::DoOpen(const std::filesystem::path &p) {
    if (doc().Load(p)) {
        RebuildView();
    } else {
        pfd::message("Load Failed", "Could not load: " + p.string(),
                     pfd::choice::ok, pfd::icon::error);
    }
}

void RackFrame::DoSave() {
    if (!doc().Path())
        return; // guarded by menu logic, but be safe
    DoSaveAs(*doc().Path());
}

void RackFrame::DoSaveAs(const std::filesystem::path &p) {
    if (!doc().Save(p)) {
        pfd::message("Save Failed", "Could not save: " + p.string(),
                     pfd::choice::ok, pfd::icon::error);
    }
}

// ---------- Unsaved-changes modal ----------

void RackFrame::DrawUnsavedModal() {
    if (show_unsaved_modal_) {
        ImGui::OpenPopup("Unsaved Changes");
        show_unsaved_modal_ = false;
    }
    if (ImGui::BeginPopupModal("Unsaved Changes", nullptr,
                               ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("You have unsaved changes. Discard them?");
        ImGui::Separator();
        if (ImGui::Button("Save First")) {
            ImGui::CloseCurrentPopup();
            if (doc().Path()) {
                DoSave();
                if (pending_ == PendingAction::OpenAfterPrompt) {
                    StartOpenDialog();
                } else if (pending_ == PendingAction::NewAfterPrompt) {
                    DoNew();
                }
                pending_ = PendingAction::None;
            } else {
                // No path yet — kick off SaveAs; PollPendingDialog handles
                // the follow-up using `pending_`.
                StartSaveAsDialog();
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Discard")) {
            ImGui::CloseCurrentPopup();
            if (pending_ == PendingAction::OpenAfterPrompt) {
                StartOpenDialog();
            } else if (pending_ == PendingAction::NewAfterPrompt) {
                DoNew();
            }
            pending_ = PendingAction::None;
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            ImGui::CloseCurrentPopup();
            pending_ = PendingAction::None;
        }
        ImGui::EndPopup();
    }
}

} // namespace augr