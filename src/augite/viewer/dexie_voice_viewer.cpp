#include "viewer_factory.h"

#include "dexie_voice_viewer.h"

namespace augr {

void DexieVoiceViewer::OnDrawMainMenuBar() {
    if (is_active() && ImGui::BeginMenu("Patch")) {
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
    VoiceViewer::OnDrawMainMenuBar();
}

DEFINE_VIEWER_FACTORY(DexieVoiceViewer, RackDoc, fm::DexieVoice)

}