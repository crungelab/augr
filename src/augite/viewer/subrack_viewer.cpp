#include <augr/core/archive.h>
#include <augr/core/archiver.h>
#include <augr/core/archiver_manufacturer.h>

#include <augite/app/app.h>

#include "subrack_viewer.h"
#include "viewer_factory.h"

#include <imgui.h>

namespace augr {

SubrackViewer::SubrackViewer(const std::string &label, RackDoc &doc,
                             Subrack &subrack)
    : DocumentViewerT<RackDoc, Subrack, SubrackView, SubrackController>(
          label, doc, subrack) {
    on_doc_save_conn_ = doc_->on_save.connect([this]() {
        SaveViewerState();
    });
    on_doc_load_conn_ = doc_->on_load.connect([this]() { OnLoaded(); });
}

void SubrackViewer::OnDestroy() {
    on_doc_save_conn_.disconnect();
    on_doc_load_conn_.disconnect();
    DocumentViewerT<RackDoc, Subrack, SubrackView,
                    SubrackController>::OnDestroy();
}

SubrackViewer::~SubrackViewer() {
    if (doc_) {
        // Capture final view state before going away. Closing a frame
        // shouldn't discard its layout — the user might reopen this
        // subrack later in the session.
        SaveViewerState();
    }
}

void SubrackViewer::Create() {
    Widget::Create();
    RebuildView();
}

void SubrackViewer::OnLoaded() {
    DestroyChildren();
    RebuildView();
}

void SubrackViewer::RebuildView() {
    view_ = std::make_unique<SubrackView>(document());
    view().set_model(model());
    view().Build();
    controller_ =
        std::make_unique<SubrackController>(document(), view(), *this);
    controller().set_model(model());

    RestoreViewerState();
}

void SubrackViewer::SaveViewerState() {
    nlohmann::json out;
    Archive archive(out);
    ArchiverManufacturer::singleton().Serialize(archive, *this);
    document().viewers_[model().uuid()] = std::move(out);
}

void SubrackViewer::RestoreViewerState() {
    auto it = document().viewers_.find(model().uuid());
    if (it == document().viewers_.end())
        return;

    Archive archive(it->second);
    ArchiverManufacturer::singleton().Deserialize(archive, *this);
}

void SubrackViewer::Draw() {
    PollPendingDialog();
    if (is_active()) {
        DrawMenuBar();
    }
    DrawUnsavedModal();
    Viewer::Draw();
}

// ---------- Dialog polling ----------

void SubrackViewer::PollPendingDialog() {
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

            if (pending_ == PendingAction::OpenAfterPrompt) {
                StartOpenDialog();
            } else if (pending_ == PendingAction::NewAfterPrompt) {
                DoNew();
            }
            pending_ = PendingAction::None;
        } else {
            pending_ = PendingAction::None;
        }
    }
}

void SubrackViewer::DrawMenuBar() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New", "Ctrl+N")) {
                if (document().IsModified()) {
                    pending_ = PendingAction::NewAfterPrompt;
                    show_unsaved_modal_ = true;
                } else {
                    DoNew();
                }
            }
            if (ImGui::MenuItem("Open...", "Ctrl+O")) {
                if (document().IsModified()) {
                    pending_ = PendingAction::OpenAfterPrompt;
                    show_unsaved_modal_ = true;
                } else {
                    StartOpenDialog();
                }
            }
            ImGui::Separator();
            bool can_save = document().IsModified() || !document().Path();
            if (ImGui::MenuItem("Save", "Ctrl+S", false, can_save)) {
                if (document().Path()) {
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

        if (ImGui::BeginMenu("Edit")) {
            const bool has_selection = controller().HasSelection();
            const bool has_clipboard = controller().HasClipboardSelection();

            if (ImGui::MenuItem("Cut", "Ctrl+X", false, has_selection)) {
                controller().Cut();
            }
            if (ImGui::MenuItem("Copy", "Ctrl+C", false, has_selection)) {
                controller().Copy();
            }
            if (ImGui::MenuItem("Paste", "Ctrl+V", false, has_clipboard)) {
                controller().Paste();
            }
            if (ImGui::MenuItem("Duplicate", "Ctrl+D", false, has_selection)) {
                controller().Duplicate();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Delete", "Del", false, has_selection)) {
                controller().DeleteSelection();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Select All", "Ctrl+A")) {
                controller().SelectAll();
            }

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

// ---------- Dialog launchers ----------

void SubrackViewer::StartOpenDialog() {
    std::string default_dir = document().Path()
                                  ? document().Path()->parent_path().string()
                                  : pfd::path::home();

    open_dialog_ = std::make_unique<pfd::open_file>(
        "Open Rack", default_dir,
        std::vector<std::string>{"Augr Rack (*.augr)", "*.augr", "All Files",
                                 "*"},
        pfd::opt::none);
}

void SubrackViewer::StartSaveAsDialog() {
    std::string default_path =
        document().Path() ? document().Path()->string() : "untitled.augr";

    save_dialog_ = std::make_unique<pfd::save_file>(
        "Save Rack As", default_path,
        std::vector<std::string>{"Augr Rack (*.augr)", "*.augr"},
        pfd::opt::force_overwrite);
}

// ---------- Document operations ----------
void SubrackViewer::DoNew() {
    App::singleton().QueueAction([this]() { document().NewDocument(); });
}

void SubrackViewer::DoOpen(const std::filesystem::path &p) {
    App::singleton().QueueAction([this, p]() {
        if (!document().Load(p)) {
            pfd::message("Load Failed", "Could not load: " + p.string(),
                         pfd::choice::ok, pfd::icon::error);
        }
    });
}

void SubrackViewer::DoSave() {
    if (!document().Path())
        return;
    DoSaveAs(*document().Path());
}

void SubrackViewer::DoSaveAs(const std::filesystem::path &p) {
    if (!document().Save(p)) {
        pfd::message("Save Failed", "Could not save: " + p.string(),
                     pfd::choice::ok, pfd::icon::error);
    }
}

// ---------- Unsaved-changes modal ----------

void SubrackViewer::DrawUnsavedModal() {
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
            if (document().Path()) {
                DoSave();
                if (pending_ == PendingAction::OpenAfterPrompt) {
                    StartOpenDialog();
                } else if (pending_ == PendingAction::NewAfterPrompt) {
                    DoNew();
                }
                pending_ = PendingAction::None;
            } else {
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

DEFINE_VIEWER_FACTORY(SubrackViewer, RackDoc, Subrack)

} // namespace augr