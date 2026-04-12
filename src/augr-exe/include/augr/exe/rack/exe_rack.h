#pragma once

#include <augr/core/midi/midi_message.h>
#include <augr/rack/rack.h>

#include <augr/exe/rack/audio_configurator.h>
#include <augr/exe/rack/audio_system.h>
#include <augr/exe/rack/midi_system.h>

namespace augr {

class AudioInputDevice;
class AudioOutputDevice;

class ExeRack : public Rack {
public:
    REFLECT_ENABLE(Rack)
    bool Create() override;
    bool Start();
    void Stop();

    // Called by AudioSystem::Callback on the audio thread
    int ProcessAudio(double streamTime, void *inbuf, void *outbuf,
                     unsigned long frames);

    // Called by MidiSystem::Callback on the MIDI thread;
    // enqueues into the action queue for processing on the audio thread
    void EnqueueMidiMessage(MidiMessage message);

private:
    bool CreateAudioInputDevice();
    bool CreateAudioOutputDevice();

    AudioSystem audio_system_{*this};
    MidiSystem midi_system_{*this};

    AudioInputDevice *audio_input_device_ = nullptr;
    AudioOutputDevice *audio_output_device_ = nullptr;

    unsigned int devNumInChans_ = 0;
    unsigned int devNumOutChans_ = 0;
};

} // namespace augr