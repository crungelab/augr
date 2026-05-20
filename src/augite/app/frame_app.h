#pragma once

#include "app.h"

namespace augr {

class FrameApp : public App {
public:
};

template <typename TFrame> class FrameAppT : public FrameApp {
public:
    FrameAppT() {
    }
    TFrame &root_frame() { return *static_cast<TFrame *>(root_frame_.get()); }
    TFrame *active_frame() { return static_cast<TFrame *>(active_frame_); }
};

} // namespace augr