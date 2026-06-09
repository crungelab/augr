#pragma once

#include <sigslot/signal.hpp>

#include "frame_app.h"

#include "../controller/subrack_controller.h"
#include "../viewer/rack_viewer.h"

namespace augr {

class RackApp : public FrameAppT<RackViewer> {
public:
    RackApp();
    bool DoCreate(CreateParams params) override;

    void Draw() override;
    void DrawMainDockspace();

    // Accessors
    static RackApp &singleton() { return *singleton_; }
    RackDoc &document() { return static_cast<RackDoc &>(*doc_); }
    Rack &rack() { return document().rack(); }
    SubrackController *active_controller() {
        return active_frame() ? &active_frame()->controller() : nullptr;
    }

    // Data members
    static RackApp *singleton_;
    sigslot::scoped_connection on_doc_unload_conn_;
};

} // namespace augr