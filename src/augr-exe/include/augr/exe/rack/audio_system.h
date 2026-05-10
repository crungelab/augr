#pragma once

#include <memory>

#include <RtAudio.h>
#include <augr/exe/rack/audio_configurator.h>

namespace augr {

class ExeRack;

class AudioSystem {
public:
    explicit AudioSystem(ExeRack &rack);
    ~AudioSystem();

    // Resolve device IDs, channel counts, sample rate, and frame size.
    // Caches the resolved config for subsequent Start() calls.
    // Call this once at app launch (or whenever device selection changes).
    bool Configure(AudioConfig &config);

    // Open and start the audio stream using the cached config.
    // Idempotent if already running. Safe to call after Stop().
    bool Start();

    // Stop, close, and destroy the stream. Idempotent.
    void Stop();

    bool IsRunning() const;

    unsigned int num_in_chans() const { return num_in_chans_; }
    unsigned int num_out_chans() const { return num_out_chans_; }

private:
    static int Callback(void *outputBuffer, void *inputBuffer,
                        unsigned int nBufferFrames, double streamTime,
                        RtAudioStreamStatus status, void *userdata);

    ExeRack &rack_;
    std::unique_ptr<RtAudio> dac_;
    AudioConfig config_{};
    bool configured_ = false;
    unsigned int num_in_chans_ = 0;
    unsigned int num_out_chans_ = 0;
};

} // namespace augr