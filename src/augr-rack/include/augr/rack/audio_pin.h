#pragma once

#include <augr/audio.h>
#include <augr/rack/pin.h>

namespace augr {

class AudioPin : public Pin {
public:
    AudioPin(Node &node, std::string name,
                   ChannelLayout layout = ChannelLayout::kMono)
        : Pin(node, name), layout_(layout) {}

    ChannelLayout layout_ = ChannelLayout::kMono;
};

class AudioOutput : public OutputT<Audio, AudioPin> {
public:
    AudioOutput(Node &node, std::string name,
                ChannelLayout layout = ChannelLayout::kMono)
        : OutputT<Audio, AudioPin>(node, name, layout) {}

    void Write(Audio audio) override {
        if (audio.layout() != layout_) {
            OutputT<Audio, AudioPin>::Write(audio.Convert(layout_));
            return;
        }
        OutputT<Audio, AudioPin>::Write(audio);
    }
};

class AudioInput : public PolyInputT<Audio, AudioPin> {
public:
    AudioInput(Node &node, std::string name,
               ChannelLayout layout = ChannelLayout::kMono)
        : PolyInputT<Audio, AudioPin>(node, name, layout) {}

    Audio Transform(Audio audio) override {
        return audio.layout() != layout_ ? audio.Convert(layout_) : audio;
    }
};

// An AudioInput variant for carrier summing — applies headroom scaling
// to reduce clipping when multiple carriers sum into one output.
// Distinct from plain AudioInput/VoltageInput, where summation (e.g.
// FM phase modulation) must remain unscaled.
class MixingAudioInput : public AudioInput {
public:
    using AudioInput::AudioInput;

    Audio Reduce() const override {
        Audio mixed = slots_[0]->Read();
        for (size_t i = 1; i < slots_.size(); ++i)
            mixed.array() += slots_[i]->Read().array();

        // Fixed headroom rather than 1/N — avoids quietening
        // single-carrier algorithms while preventing multi-carrier clipping.
        constexpr float kHeadroom = 0.25f;
        mixed.array() *= kHeadroom;
        return mixed;
    }

    REFLECT_ENABLE(AudioInput)
};

} // namespace augr