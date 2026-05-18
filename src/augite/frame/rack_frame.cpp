#include <augr/core/archiver_manufacturer.h>
#include <augr/core/archive.h>
#include <augr/core/archiver.h>

#include <augr/rack/rack_doc.h>

#include "rack_frame.h"

namespace augr {

RackFrame::RackFrame(const std::string &label)
    : FrameT<RackDoc, RackView, RackController>(label) {
    doc_ = std::make_unique<RackDoc>();

    // Install view hooks BEFORE NewDocument, so the load-hook
    // gets invoked on the initial document. RackFrame owns both doc_
    // and view_, so the `this` captures are valid for the lifetime of
    // the doc_.
    save_view_token_ = doc_->AddSaveHook("view", [this]() {
        return ViewToJson();
    });
    load_view_token_ = doc_->AddLoadHook("view", [this](const nlohmann::json &j) {
        RebuildView();
        if (!j.empty()) {
            ViewFromJson(j);
        }
    });

    doc().NewDocument();
    // RebuildView is now triggered by the load hook on NewDocument.
}

RackFrame::~RackFrame() {
    if (doc_) {
        doc_->RemoveSaveHook(save_view_token_);
        doc_->RemoveLoadHook(load_view_token_);
    }
}

//void RackFrame::RebuildView() { view_ = std::make_unique<RackView>(doc()); }

void RackFrame::RebuildView() {
    view_ = std::make_unique<RackView>(doc());
    view().Build();  // construct widget tree now so view archiver has something to load into
    controller_ = std::make_unique<RackController>(doc(), view());
}

nlohmann::json RackFrame::ViewToJson() {
    if (!view_) return nlohmann::json();

    auto &mfr = ArchiverManufacturer::singleton();
    auto *factory = mfr.FindFactory(std::type_index(typeid(*view_)));
    if (!factory) {
        std::cerr << "No archiver factory for RackView\n";
        return nlohmann::json();
    }
    std::unique_ptr<Archiver> archiver(factory->Produce(*view_));
    nlohmann::json out;
    Archive archive(out);
    archiver->Save(archive);
    return out;
}

void RackFrame::ViewFromJson(const nlohmann::json &j) {
    if (!view_) return;

    auto &mfr = ArchiverManufacturer::singleton();
    auto *factory = mfr.FindFactory(std::type_index(typeid(*view_)));
    if (!factory) return;

    std::unique_ptr<Archiver> archiver(factory->Produce(*view_));
    nlohmann::json local = j;
    Archive archive(local);
    archiver->Load(archive);
}

void RackFrame::Draw() {
    PollPendingDialog();
    DrawMenuBar();
    DrawUnsavedModal();
    Frame::Draw();
}

void RackFrame::Begin() {
    ImGuiWindowFlags graph_flags =
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoBringToFrontOnFocus;

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
    // Load hook fires from inside NewDocument and rebuilds the view.
}

void RackFrame::DoOpen(const std::filesystem::path &p) {
    if (!doc().Load(p)) {
        pfd::message("Load Failed", "Could not load: " + p.string(),
                     pfd::choice::ok, pfd::icon::error);
    }
    // Load hook handles RebuildView and view JSON deserialization.
}

void RackFrame::DoSave() {
    if (!doc().Path())
        return;
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