#pragma once

#include <augr/rack/rack_doc.h>
#include <augr/rack/voice/voice.h>

#include "../controller/subrack_controller.h"
#include "../view/subrack_view.h"

#include "subrack_viewer.h"

namespace augr {
class VoiceViewer
    : public ViewerT<RackDoc, Voice, SubrackView, SubrackController, SubrackViewer> {
public:
    using ViewerT<RackDoc, Voice, SubrackView, SubrackController, SubrackViewer>::ViewerT;
};

} // namespace augr