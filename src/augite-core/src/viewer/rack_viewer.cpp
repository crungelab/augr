#include <augr/archiver_manufacturer.h>
#include <augr/archive.h>
#include <augr/archiver.h>

#include <augr/rack/rack_doc.h>
#include <augr/rack/module/audio_device.h>
#include <augite/viewer/rack_viewer.h>

namespace augr {


RackViewer::RackViewer(const std::string &label, RackDoc &doc, Rack &rack)
    : SubrackViewer(label, doc, rack) {
    docked_ = true;
}

RackViewer::~RackViewer() {
}

void RackViewer::DrawControls() {
    SubrackViewer::DrawControls();

    auto audio = rack().audio_output_device_->audio_in_->Read();
    float level = rack().vu_level_.load(std::memory_order_relaxed);
    ImGui::ProgressBar(level, ImVec2(200, 16), "");
    //ImGui::ProgressBar(rack().vu_level_, ImVec2(200, 16), "");
}

void RackViewer::OnLoaded() {
    set_model(document().model());
    SubrackViewer::OnLoaded();
}

void RackViewer::OnDrawMainMenuBar() {
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
    SubrackViewer::OnDrawMainMenuBar();
}

void RackViewer::Begin() {
    ImGuiWindowFlags graph_flags =
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::Begin(label_.c_str(), nullptr, graph_flags);
}

} // namespace augr