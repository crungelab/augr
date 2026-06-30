#pragma once

#include <augr/rack/library/scope_module.h>

#include "console_view.h"

namespace augr {

class ScopeView : public ViewT<ScopeModule, ConsoleView> {
public:
    using ViewT<ScopeModule, ConsoleView>::ViewT;
    void Draw() override;

    // Capture this many multiples of display_window so the trigger can
    // land in the first 1/kCaptureMultiplier of the capture and still
    // leave a full display_window of samples after it. Prevents the
    // right edge of the waveform from flickering as the trigger position
    // varies frame to frame.
    static constexpr std::size_t kCaptureMultiplier = 3;
};

} // namespace augr