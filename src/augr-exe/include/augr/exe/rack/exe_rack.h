#pragma once

// #include <augr/core/midi/midi_message.h>
#include <augr/rack/rack.h>

#include <augr/exe/rack/audio_configurator.h>
#include <augr/exe/rack/audio_system.h>
#include <augr/exe/rack/midi_system.h>

namespace augr {

class ExeRack : public Rack {
public:
    ExeRack() = default;
    REFLECT_ENABLE(Rack)
    void Create(Part *owner = nullptr) override;
    bool Start();
    void Stop();

    // Called by AudioSystem::Callback on the audio thread
    int ProcessAudio(double streamTime, void *inbuf, void *outbuf,
                     unsigned long frames);

private:
    AudioSystem audio_system_{*this};
    MidiSystem midi_system_{*this};

    unsigned int devNumInChans_ = 0;
    unsigned int devNumOutChans_ = 0;
};

} // namespace augr