#include <fstream>

#include <imgui.h>

#include <augr/archiver_factory.h>
#include <augr/archiver_manufacturer.h>

#include <augr/fm/dx7_algorithm.h>
#include <augr/fm/dx7_patch.h>
#include <augite/app/app.h>

#include <augite/archiver/subrack_viewer_archiver.h>

#include <augite/viewer/viewer_factory.h>
#include <augite/fm/viewer/dexie_voice_viewer.h>

namespace augr {

void DexieVoiceViewer::Draw() {
    PollSysexDialog();
    DrawPatchBrowser();
    VoiceViewer::Draw();
}

void DexieVoiceViewer::OnDrawMainMenuBar() {
    if (is_active() && ImGui::BeginMenu("Patch")) {
        if (ImGui::MenuItem("Load Sysex...")) {
            sysex_dialog_ = std::make_unique<pfd::open_file>(
                "Load DX7 Sysex", pfd::path::home(),
                std::vector<std::string>{"DX7 Sysex (*.syx)", "*.syx",
                                         "All Files", "*"},
                pfd::opt::none);
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Browse Patches...", nullptr,
                            show_patch_browser_, !cartridge_.empty())) {
            show_patch_browser_ = !show_patch_browser_;
        }
        ImGui::EndMenu();
    }
    VoiceViewer::OnDrawMainMenuBar();
}

void DexieVoiceViewer::PollSysexDialog() {
    if (sysex_dialog_ && sysex_dialog_->ready(0)) {
        auto result = sysex_dialog_->result();
        sysex_dialog_.reset();
        if (!result.empty())
            DoLoadSysex(result.front());
    }
}

void DexieVoiceViewer::DrawPatchBrowser() {
    if (!show_patch_browser_ || cartridge_.empty())
        return;

    ImGui::SetNextWindowSize(ImVec2(320, 520), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("DX7 Patches", &show_patch_browser_)) {
        // Patch list
        ImGui::BeginChild("PatchList", ImVec2(0, 360), true);
        for (int i = 0; i < static_cast<int>(cartridge_.size()); ++i) {
            const auto& patch = cartridge_[i];
            const std::string label =
                std::to_string(i + 1) + ". " + patch.name;
            if (ImGui::Selectable(label.c_str(), selected_patch_ == i))
                selected_patch_ = i;
            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
                DoLoadPatch(patch);
        }
        ImGui::EndChild();

        // Algorithm diagram for selected patch
        if (selected_patch_ >= 0) {
            ImGui::Separator();
            DrawAlgorithmDiagram(cartridge_[selected_patch_].algorithm);
        }

        ImGui::Separator();
        const bool has_selection = selected_patch_ >= 0;
        if (ImGui::Button("Load", ImVec2(80, 0)) && has_selection)
            DoLoadPatch(cartridge_[selected_patch_]);
        ImGui::SameLine();
        if (ImGui::Button("Close", ImVec2(80, 0)))
            show_patch_browser_ = false;
    }
    ImGui::End();
}

void DexieVoiceViewer::DrawAlgorithmDiagram(int algorithm) {
    const fm::Dx7AlgorithmDef& def = fm::GetDx7Algorithm(algorithm);

    ImGui::Text("Algorithm %d", algorithm + 1);

    ImGui::Text("Carriers:");
    ImGui::SameLine();
    for (int i = 0; i < 6; ++i) {
        if (def.is_carrier[i]) {
            ImGui::Text("OP%d", i + 1);
            ImGui::SameLine();
        }
    }
    ImGui::NewLine();

    ImGui::Text("Routes:");
    for (int r = 0; r < def.route_count; ++r)
        ImGui::Text("  OP%d -> OP%d",
                    def.routes[r].src + 1,
                    def.routes[r].dst + 1);
}

void DexieVoiceViewer::DoLoadSysex(const std::filesystem::path& p) {
    std::ifstream f(p, std::ios::binary);
    if (!f) {
        pfd::message("Load Failed", "Could not open: " + p.string(),
                     pfd::choice::ok, pfd::icon::error);
        return;
    }

    const std::vector<uint8_t> sysex(std::istreambuf_iterator<char>(f), {});
    auto patches = fm::ParseDx7Cartridge(sysex);

    if (patches.empty()) {
        pfd::message("Load Failed", "No patches found in: " + p.string(),
                     pfd::choice::ok, pfd::icon::error);
        return;
    }

    cartridge_          = std::move(patches);
    selected_patch_     = 0;
    show_patch_browser_ = true;
}

void DexieVoiceViewer::DoLoadPatch(const fm::Dx7Patch& patch) {
    auto &rack = model().rack();
    rack.EnqueueAction([this, patch]() {
        model().LoadPatch(patch);
    });
}

DEFINE_VIEWER_FACTORY(DexieVoiceViewer, RackDoc, fm::DexieVoice)

class DexieVoiceViewerArchiver : public SubrackViewerArchiver {};
DEFINE_ARCHIVER_FACTORY(DexieVoiceViewerArchiver, DexieVoiceViewer, "DexieVoiceViewer")

} // namespace augr
