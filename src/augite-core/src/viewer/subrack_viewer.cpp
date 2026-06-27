#include <augr/archive.h>
#include <augr/archiver.h>
#include <augr/archiver_manufacturer.h>

#include <augite/app/app.h>
#include <augite/widget/widget_builder.h>

#include <augite/viewer/subrack_viewer.h>
#include <augite/viewer/viewer_factory.h>

#include <imgui.h>

namespace augr {

SubrackViewer::SubrackViewer(const std::string &label, RackDoc &doc,
                             Subrack &subrack)
    : DocumentViewerT<RackDoc, Subrack, SubrackView, SubrackController>(
          label, doc, subrack) {}

SubrackViewer::~SubrackViewer() {}

void SubrackViewer::RebuildView() {
    view_ = std::make_unique<SubrackView>(document());
    view().set_model(model());
    view().Build();

    BuildControls();

    controller_ =
        std::make_unique<SubrackController>(document(), view(), *this);
    controller().set_model(model());
}

void SubrackViewer::BuildControls() {
    ModelWidgetBuilder builder;
    controls_root_ = new Widget(); // dummy root to hold the real root's children
    AddChild(Widget::Ptr(controls_root_)); // take ownership of the dummy root
    builder.BuildChildren(*controls_root_, *model().controls_);
}

void SubrackViewer::Draw() {
    PollPendingDialog();
    DrawUnsavedModal();
    Viewer::Draw();
    if (is_active()) {
        DrawConsole();
    }
}

void SubrackViewer::DrawConsole() {
    if (ImGui::Begin("Console")) {
        DrawControls();
        ImGui::End();
    }
}

void SubrackViewer::DrawControls() {
    controls_root_->Draw();
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

void SubrackViewer::OnDrawMainMenuBar() {
    if (is_active() && ImGui::BeginMenu("Edit")) {
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
    Viewer::OnDrawMainMenuBar();
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
    App::singleton().EnqueueAction([this]() { document().NewDocument(); });
}

void SubrackViewer::DoOpen(const std::filesystem::path &p) {
    App::singleton().EnqueueAction([this, p]() {
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