#pragma once

#include <augr/core/audio.h>
#include <augr/core/rack/connector.h>

namespace augr {

class AudioConnector : public Connector {
public:
    AudioConnector(Node &node, std::string name,
                   ChannelLayout layout = ChannelLayout::kMono)
        : Connector(node, name), layout_(layout) {}

    ChannelLayout layout_ = ChannelLayout::kMono;
};

class AudioOutput : public OutputT<Audio, AudioConnector> {
public:
    AudioOutput(Node &node, std::string name,
                ChannelLayout layout = ChannelLayout::kMono)
        : OutputT<Audio, AudioConnector>(node, name, layout) {}

    void Write(Audio audio) override {
        if (audio.layout_ != layout_) {
            OutputT<Audio, AudioConnector>::Write(audio.Convert(layout_));
            return;
        }
        OutputT<Audio, AudioConnector>::Write(audio);
    }
};

class AudioInput : public InputT<Audio, AudioConnector> {
public:
    AudioInput(Node &node, std::string name,
               ChannelLayout layout = ChannelLayout::kMono)
        : InputT<Audio, AudioConnector>(node, name, layout) {}

    void Disconnect(Connection &connection) override {
        InputT<Audio, AudioConnector>::Disconnect(connection);
        Write(Audio());
    }

    void Write(Audio audio) override {
        if (audio.layout_ != layout_) {
            InputT<Audio, AudioConnector>::Write(audio.Convert(layout_));
            return;
        }
        InputT<Audio, AudioConnector>::Write(audio);
    }
};

} // namespace augr