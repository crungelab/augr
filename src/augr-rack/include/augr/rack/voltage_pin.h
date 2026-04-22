#pragma once

#include <augr/rack/audio_pin.h>

namespace augr {

// A Voltage is a mono Audio buffer carrying a control signal rather than audio.
// Using a typedef keeps the buffer machinery (xtensor, frames, sample_rate) shared.
using Voltage = Audio;

class VoltageInput : public AudioInput {
public:
    VoltageInput(Node &owner, const std::string &name)
        : AudioInput(owner, name, ChannelLayout::kMono) {}

    REFLECT_ENABLE(AudioInput)
};

class VoltageOutput : public AudioOutput {
public:
    VoltageOutput(Node &owner, const std::string &name)
        : AudioOutput(owner, name, ChannelLayout::kMono) {}

    REFLECT_ENABLE(AudioOutput)
};

} // namespace augr