// augr/fm/dexie_voice_viewer.h
#pragma once

#include <augr/fm/dx7_patch.h>
#include <augr/rack/rack_doc.h>
#include <augr/fm/dexie_voice.h>

#include <augite/controller/subrack_controller.h>
#include <augite/view/subrack_view.h>

#include <augite/viewer/voice_viewer.h>

#include <portable-file-dialogs.h>

#include <vector>

namespace augr {

class DexieVoiceViewer
    : public ViewerT<RackDoc, fm::DexieVoice, SubrackView, SubrackController, VoiceViewer> {
public:
    using ViewerT<RackDoc, fm::DexieVoice, SubrackView, SubrackController, VoiceViewer>::ViewerT;

    void Draw() override;
    void OnDrawMainMenuBar() override;

private:
    // Patch browser state
    std::vector<fm::Dx7Patch> cartridge_;
    int                       selected_patch_ = -1;
    bool                      show_patch_browser_ = false;

    // Sysex file dialog — reuses open_dialog_ from SubrackViewer for
    // document open; this is a separate dialog for sysex loading only.
    std::unique_ptr<pfd::open_file> sysex_dialog_;

    void PollSysexDialog();
    void DrawPatchBrowser();
    void DrawAlgorithmDiagram(int algorithm);

    void DoLoadSysex(const std::filesystem::path& p);
    void DoLoadPatch(const fm::Dx7Patch& patch);
};

} // namespace augr