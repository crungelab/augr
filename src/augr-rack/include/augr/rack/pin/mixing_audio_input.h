#pragma once

#include <augr/rack/pin/audio_pin.h>

namespace augr {

// An AudioInput variant for carrier summing — applies headroom scaling
// to reduce clipping when multiple carriers sum into one output.
// Distinct from plain AudioInput/VoltageInput, where summation (e.g.
// FM phase modulation) must remain unscaled.
class MixingAudioInput : public AudioInput {
public:
    using AudioInput::AudioInput;

    Audio Reduce() const override;
};

} // namespace augr