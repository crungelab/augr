#pragma once

#include "module_widget.h"

namespace augr {

class ScopeModule;

class ScopeWidget : public ModuleWidgetT<ScopeModule> {
public:
    explicit ScopeWidget(ScopeModule& model) : ModuleWidgetT<ScopeModule>(model) {}

    void DrawDockContent() override;    // scope-specific node rendering

    // Capture this many multiples of display_window so the trigger can
    // land in the first 1/kCaptureMultiplier of the capture and still
    // leave a full display_window of samples after it. Prevents the
    // right edge of the waveform from flickering as the trigger position
    // varies frame to frame.
    static constexpr std::size_t kCaptureMultiplier = 3;
};

} // namespace augr