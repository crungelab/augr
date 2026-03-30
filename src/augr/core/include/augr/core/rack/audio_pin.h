#pragma once

#include <augr/core/audio.h>
#include <augr/core/rack/pin.h>

namespace augr {

typedef PinT<Audio> AudioPinBase;

class AudioPin : public AudioPinBase {
public:
    AudioPin(Node &node, std::string name,
             ChannelLayout layout = ChannelLayout::kMono)
        : AudioPinBase(node, name), layout_(layout) {}

    void Disconnect(Pin &input) override {
        AudioPinBase::Disconnect(input);
        PinT<Audio> *input_pin = dynamic_cast<PinT<Audio> *>(&input);
        input_pin->Write(
            Audio()); // Write silence to the input pin when disconnected
    }

    void Write(Audio audio) override {
        if (audio.layout_ != layout_) {
            AudioPinBase::Write(audio.Convert(layout_));
            return;
        }
        AudioPinBase::Write(audio);
    }
    // Data members
    ChannelLayout layout_ = ChannelLayout::kMono;
};

} // namespace augr