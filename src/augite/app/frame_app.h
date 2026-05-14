#pragma once

#include "app.h"

namespace augr {

class FrameApp : public App {
public:
};

template <typename TFrame> class FrameAppT : public FrameApp {
public:
    FrameAppT() {
        //frame_ = new TFrame("FrameApp");
    }
    TFrame &frame() { return *static_cast<TFrame *>(frame_); }
};

} // namespace augr