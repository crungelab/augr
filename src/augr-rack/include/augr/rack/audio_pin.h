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
        if (audio.layout_ != layout_) {
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

    void Disconnect(Connection &connection) override {
        PolyInputT<Audio, AudioPin>::Disconnect(connection);
        Write(Audio());
    }

    void Write(Audio audio) override {
        if (audio.layout_ != layout_) {
            PolyInputT<Audio, AudioPin>::Write(audio.Convert(layout_));
            return;
        }
        PolyInputT<Audio, AudioPin>::Write(audio);
    }
};

} // namespace augr