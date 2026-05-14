#pragma once

#include "frame_app.h"

//#include <augr/rack/rack.h>
//#include <augr/rack/rack_doc.h>

#include "../frame/rack_frame.h"

namespace augr {

//class RackView;

class RackApp : public FrameAppT<RackFrame> {
public:
    RackApp();
    void Draw() override;
    void DrawMainDockspace();
    // Accessors
    RackDoc &doc() { return frame().doc(); }
    Rack &rack() { return frame().rack(); }

    // Data members
    static RackApp *singleton_;
    //RackDoc doc_;
};

} // namespace augr