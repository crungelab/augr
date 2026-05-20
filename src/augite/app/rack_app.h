#pragma once

#include "frame_app.h"

#include "../frame/rack_frame.h"
#include "../controller/subrack_controller.h"

namespace augr {

class RackApp : public FrameAppT<RackFrame> {
public:
    RackApp();
    void Draw() override;
    void DrawMainDockspace();
    // Accessors
    static RackApp &singleton() { return *singleton_; }
    RackDoc &doc() { return root_frame().doc(); }
    Rack &rack() { return root_frame().rack(); }
    SubrackController * active_controller() { return active_frame() ? &active_frame()->controller() : nullptr; }

    // Data members
    static RackApp *singleton_;
};

} // namespace augr