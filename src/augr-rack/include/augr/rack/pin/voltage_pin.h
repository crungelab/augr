#pragma once

#include <augr/rack/pin/audio_pin.h>

namespace augr {

// A Voltage is a mono Audio buffer carrying a control signal rather than audio.
// Using a typedef keeps the buffer machinery (xtensor, frames, sample_rate) shared.
using Voltage = Audio;

class VoltageInput : public AudioInput {
public:
    VoltageInput(Node &node, const std::string &name)
        : AudioInput(node, name, ChannelLayout::kMono) {}

    REFLECT_ENABLE(AudioInput)
};

class VoltageOutput : public AudioOutput {
public:
    VoltageOutput(Node &node, const std::string &name)
        : AudioOutput(node, name, ChannelLayout::kMono) {}

    REFLECT_ENABLE(AudioOutput)
};

} // namespace augr