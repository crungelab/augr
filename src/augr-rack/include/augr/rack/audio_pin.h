#pragma once

#include <augr/core/audio.h>
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

} // namespace augr