#pragma once

#include <RtAudio.h>
#include <augr/exe/rack/audio_configurator.h>

namespace augr {

class ExeRack;

class AudioSystem {
public:
    explicit AudioSystem(ExeRack &rack);
    ~AudioSystem();

    bool Create(AudioConfig &config);
    bool Start();
    void Stop();

    unsigned int num_in_chans() const { return num_in_chans_; }
    unsigned int num_out_chans() const { return num_out_chans_; }

private:
    static int Callback(void *outputBuffer, void *inputBuffer,
                        unsigned int nBufferFrames, double streamTime,
                        RtAudioStreamStatus status, void *userdata);

    ExeRack &rack_;
    RtAudio *dac_ = nullptr;
    unsigned int num_in_chans_ = 0;
    unsigned int num_out_chans_ = 0;
};

} // namespace augr