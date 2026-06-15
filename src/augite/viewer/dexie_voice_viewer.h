#pragma once

#include <augr/rack/rack_doc.h>
#include <augr/fm/dexie_voice.h>

#include "../controller/subrack_controller.h"
#include "../view/subrack_view.h"

#include "voice_viewer.h"

namespace augr {
class DexieVoiceViewer
    : public DocumentViewerT<RackDoc, fm::DexieVoice, SubrackView, SubrackController, VoiceViewer> {
public:
    using DocumentViewerT<RackDoc, fm::DexieVoice, SubrackView, SubrackController, VoiceViewer>::DocumentViewerT;

    void OnDrawMainMenuBar() override;
};

} // namespace augr