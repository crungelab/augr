#include <augr/core/archive.h>
#include <augr/core/archiver.h>
#include <augr/core/archiver_manufacturer.h>

#include "subrack_viewer.h"

#include <imgui.h>

namespace augr {

SubrackViewer::SubrackViewer(RackDoc &doc, Subrack &subrack,
                           const std::string &label)
    : DocumentViewerT<RackDoc, SubrackView, SubrackController>(label, doc),
      subrack_(&subrack) {
    // Install view hooks. The save hook pushes this frame's current
    // view state into views_; the load hook pulls it back out after
    // the doc has been replaced.
    save_view_token_ = doc_->AddSaveHook([this](nlohmann::json &) {
        document().views_[subrack_->uuid()] = ViewToJson();
    });
    load_view_token_ = doc_->AddLoadHook([this](const nlohmann::json &) {
        OnLoaded();
    });
}

SubrackViewer::~SubrackViewer() {
    if (doc_) {
        // Capture final view state before going away. Closing a frame
        // shouldn't discard its layout — the user might reopen this
        // subrack later in the session.
        document().views_[subrack_->uuid()] = ViewToJson();

        doc_->RemoveSaveHook(save_view_token_);
        doc_->RemoveLoadHook(load_view_token_);
    }
}

void SubrackViewer::Create(Widget *parent) {
    Widget::Create(parent);
    RebuildView();
}

void SubrackViewer::OnLoaded() {
    DestroyChildren();
    RebuildView();
}

void SubrackViewer::RebuildView() {
    view_ = std::make_unique<SubrackView>(document());
    view().set_model(subrack());
    view().Build();
    controller_ = std::make_unique<SubrackController>(document(), view(), *this);
    controller().set_model(subrack());

    // If we have a cached view state for this subrack, apply it.
    auto it = document().views_.find(subrack_->uuid());
    if (it != document().views_.end()) {
        ViewFromJson(it->second);
    }
}

void SubrackViewer::Draw() {
    PollPendingDialog();
    if (is_active()) {
        DrawMenuBar();
    }
    DrawUnsavedModal();
    Frame::Draw();
}

void SubrackViewer::Begin() {
    // Use a unique ImGui window ID per subrack so multiple frames
    // (different subracks) don't collide. The subrack pointer is stable
    // for the frame's lifetime.
    char title[128];
    std::snprintf(title, sizeof(title), "%s###subrack_%p",
                  label_.empty() ? "Subrack" : label_.c_str(),
                  static_cast<void *>(subrack_));
    bool p_open = true;
    ImGui::Begin(title, &p_open, ImGuiWindowFlags_NoCollapse);
    if (!p_open) {
        // User closed the window; close the frame.
        Destroy();
    }
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
    std::string default_dir =
        document().Path() ? document().Path()->parent_path().string() : pfd::path::home();

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
    document().NewDocument();
    // Load hook fires from inside NewDocument and rebuilds the view.
}

void SubrackViewer::DoOpen(const std::filesystem::path &p) {
    if (!document().Load(p)) {
        pfd::message("Load Failed", "Could not load: " + p.string(),
                     pfd::choice::ok, pfd::icon::error);
    }
    // Load hook handles RebuildView and view JSON deserialization.
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

nlohmann::json SubrackViewer::ViewToJson() {
    if (!view_)
        return nlohmann::json();

    auto &mfr = ArchiverManufacturer::singleton();
    auto *factory = mfr.FindFactory(std::type_index(typeid(*view_)));
    if (!factory) {
        std::cerr << "No archiver factory for SubrackView\n";
        return nlohmann::json();
    }
    std::unique_ptr<Archiver> archiver(factory->Produce(*view_));
    nlohmann::json out;
    Archive archive(out);
    archiver->Save(archive);
    return out;
}

void SubrackViewer::ViewFromJson(const nlohmann::json &j) {
    if (!view_)
        return;

    auto &mfr = ArchiverManufacturer::singleton();
    auto *factory = mfr.FindFactory(std::type_index(typeid(*view_)));
    if (!factory)
        return;

    std::unique_ptr<Archiver> archiver(factory->Produce(*view_));
    nlohmann::json local = j;
    Archive archive(local);
    archiver->Load(archive);
}

} // namespace augr