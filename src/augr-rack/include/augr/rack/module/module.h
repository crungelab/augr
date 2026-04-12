#pragma once

#include <vector>

#include <augr/rack/audio_pin.h>
#include <augr/rack/midi_pin.h>
#include <augr/rack/node.h>

namespace augr {

class Module : public Node {
public:
    //
    virtual bool Create(Part &owner) override;
    virtual Audio ProcessAudio(Audio input = Audio()) { return Audio(); }
    virtual void Process() {}
    // Data members
    const char *label_ = nullptr;
    AudioInput *audio_in_ = nullptr;
    AudioOutput *audio_out_ = nullptr;
    MidiInput *midi_in_ = nullptr;
    MidiOutput *midi_out_ = nullptr;

    REFLECT_ENABLE(Node)
};

} // namespace augr