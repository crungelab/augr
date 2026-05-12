#include <augr/core/archive.h>
#include <augr/core/archiver.h>
#include <augr/core/archiver_factory.h>
#include <augr/core/archiver_manufacturer.h>
#include <augr/core/model_manufacturer.h>

#include <augr/rack/archiver/graph_archiver.h>
#include <augr/rack/archiver/module_archiver.h>
#include <augr/rack/archiver/rack_archiver.h>
#include <augr/rack/module/module.h>
#include <augr/rack/rack_doc.h>

#include <augr/rack/module/audio_device.h>
#include <augr/rack/module/midi_device.h>

#include <augr/faust/faust_dsp.h>
#include <augr/faust/faust_dsp_ui.h>

#include <augr/exe/rack/exe_rack.h>

#include <augite/app/app.h>
#include <augite/view/rack_view.h>
#include <augite/widget/widget.h>
#include <augite/widget/widget_manufacturer.h>

#include <nlohmann/json.hpp>
#include <portable-file-dialogs.h>

#include <filesystem>
#include <iostream>
#include <memory>

#include "bubble_dsp.h"

using namespace augr;

class BubbleDspImpl : public BubbleDsp {
public:
    REFLECT_ENABLE(BubbleDsp)
};
DEFINE_MODEL_FACTORY(BubbleDspImpl, "BubbleDsp", "Faust")

class BubbleDspArchiver : public ModuleArchiver {};
DEFINE_ARCHIVER_FACTORY(BubbleDspArchiver, BubbleDspImpl, "BubbleDsp")

class ExeRackArchiver : public RackArchiver {};
DEFINE_ARCHIVER_FACTORY(ExeRackArchiver, ExeRack, "Rack")

class AudioInputDeviceArchiver : public ModuleArchiver {};
DEFINE_ARCHIVER_FACTORY(AudioInputDeviceArchiver, AudioInputDevice,
                        "AudioInputDevice")

class AudioOutputDeviceArchiver : public ModuleArchiver {};
DEFINE_ARCHIVER_FACTORY(AudioOutputDeviceArchiver, AudioOutputDevice,
                        "AudioOutputDevice")

class MidiInputDeviceArchiver : public ModuleArchiver {};
DEFINE_ARCHIVER_FACTORY(MidiInputDeviceArchiver, MidiInputDevice,
                        "MidiInputDevice")

class MidiOutputDeviceArchiver : public ModuleArchiver {};
DEFINE_ARCHIVER_FACTORY(MidiOutputDeviceArchiver, MidiOutputDevice,
                        "MidiOutputDevice")

//DEFINE_MODEL_FACTORY(ExeRack, "Rack", "Rack")
DEFINE_MODEL_FACTORY(AudioInputDevice, "AudioInputDevice", "Rack")
DEFINE_MODEL_FACTORY(AudioOutputDevice, "AudioOutputDevice", "Rack")
DEFINE_MODEL_FACTORY(MidiInputDevice, "MidiInputDevice", "Rack")
DEFINE_MODEL_FACTORY(MidiOutputDevice, "MidiOutputDevice", "Rack")

class MyApp : public App {
public:
    MyApp() {
        doc_ = std::make_unique<RackDoc>();
        doc_->NewDocument();
        RebuildView();
    }

    void Draw() override {
        PollPendingDialog();
        DrawMenuBar();
        DrawUnsavedModal();
        view_->Draw();
        //App::Draw();
    }

    void DrawMenuBar() {
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

    void StartOpenDialog() {
        std::string default_dir = doc().Path()
                                      ? doc().Path()->parent_path().string()
                                      : pfd::path::home();

        open_dialog_ = std::make_unique<pfd::open_file>(
            "Open Rack", default_dir,
            std::vector<std::string>{"Augr Rack (*.augr)", "*.augr",
                                     "All Files", "*"},
            pfd::opt::none);
    }

    void StartSaveAsDialog() {
        std::string default_path =
            doc().Path() ? doc().Path()->string() : "untitled.augr";

        save_dialog_ = std::make_unique<pfd::save_file>(
            "Save Rack As", default_path,
            std::vector<std::string>{"Augr Rack (*.augr)", "*.augr"},
            pfd::opt::force_overwrite);
    }

    // ---------- Dialog polling ----------

    void PollPendingDialog() {
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

    // ---------- Document operations ----------

    void DoNew() {
        doc().NewDocument();
        RebuildView();
    }

    void DoOpen(const std::filesystem::path &p) {
        if (doc().Load(p)) {
            RebuildView();
        } else {
            pfd::message("Load Failed", "Could not load: " + p.string(),
                         pfd::choice::ok, pfd::icon::error);
        }
    }

    void DoSave() {
        if (!doc().Path())
            return; // guarded by menu logic, but be safe
        DoSaveAs(*doc().Path());
    }

    void DoSaveAs(const std::filesystem::path &p) {
        if (!doc().Save(p)) {
            pfd::message("Save Failed", "Could not save: " + p.string(),
                         pfd::choice::ok, pfd::icon::error);
        }
    }

    // ---------- Unsaved-changes modal ----------

    void DrawUnsavedModal() {
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

    // ---------- Accessors ----------

    Rack &rack() { return doc_->rack(); }
    RackDoc &doc() { return *doc_; }

    // ---------- Data members ----------
private:
    void RebuildView() { view_ = std::make_unique<RackView>(doc_->rack()); }

    std::unique_ptr<RackDoc> doc_;
    std::unique_ptr<RackView> view_;

    enum class PendingAction {
        None,
        Open,
        SaveAs,
        NewAfterPrompt,
        OpenAfterPrompt
    };

    std::unique_ptr<pfd::open_file> open_dialog_;
    std::unique_ptr<pfd::save_file> save_dialog_;

    PendingAction pending_ = PendingAction::None;
    bool show_unsaved_modal_ = false;
};

int main(int, char **) {
    REGISTER_MODEL_FACTORY(ExeRack);
    REGISTER_MODEL_FACTORY(AudioInputDevice);
    REGISTER_MODEL_FACTORY(AudioOutputDevice);
    REGISTER_MODEL_FACTORY(MidiInputDevice);
    REGISTER_MODEL_FACTORY(MidiOutputDevice);
    REGISTER_MODEL_FACTORY(BubbleDspImpl);

    REGISTER_ARCHIVER_FACTORY(BubbleDspArchiver);
    REGISTER_ARCHIVER_FACTORY(ExeRackArchiver);
    REGISTER_ARCHIVER_FACTORY(AudioInputDeviceArchiver);
    REGISTER_ARCHIVER_FACTORY(AudioOutputDeviceArchiver);
    REGISTER_ARCHIVER_FACTORY(MidiInputDeviceArchiver);
    REGISTER_ARCHIVER_FACTORY(MidiOutputDeviceArchiver);

    auto *app = new MyApp();
    auto &rack = app->rack();

    app->Run(augr::Window::RunParams("Augr Bubble"));

    return 0;
}