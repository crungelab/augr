#include <augr/core/audio.h>

#include <augr/exe/rack/audio_system.h>
#include <augr/exe/rack/exe_rack.h>

namespace augr {

#define FORMAT RTAUDIO_FLOAT32
#define SCALE  1.0

AudioSystem::AudioSystem(ExeRack &rack) : rack_(rack) {}

AudioSystem::~AudioSystem() {
    Stop();
    if (dac_) {
        if (dac_->isStreamOpen())
            dac_->closeStream();
        delete dac_;
        dac_ = nullptr;
    }
}

int AudioSystem::Callback(void *outputBuffer, void *inputBuffer,
                          unsigned int nBufferFrames, double streamTime,
                          RtAudioStreamStatus status, void *userdata) {
    return static_cast<ExeRack *>(userdata)->ProcessAudio(
        streamTime, inputBuffer, outputBuffer, nBufferFrames);
}

bool AudioSystem::Create(AudioConfig &config) {
    dac_ = new RtAudio(
        RtAudio::Api::UNSPECIFIED,
        [](RtAudioErrorType type, const std::string &errorText) {
            spdlog::error("RtAudio error type={} message={}",
                          static_cast<int>(type), errorText);
        });

    if (dac_->getDeviceCount() < 1) {
        spdlog::error("AudioSystem: no audio devices found");
        return false;
    }

    AudioConfigurator configurator(*dac_);
    if (!configurator.configure(config)) {
        spdlog::error("AudioSystem: failed to configure audio devices");
        return false;
    }

    RtAudio::StreamParameters oParams{};
    oParams.deviceId    = config.outputDeviceId;
    oParams.nChannels   = config.outputChannels;
    oParams.firstChannel = 0;

    RtAudio::StreamParameters iParams{};
    if (config.enableInput) {
        iParams.deviceId     = config.inputDeviceId;
        iParams.nChannels    = config.inputChannels;
        iParams.firstChannel = 0;
    }

    num_in_chans_  = config.enableInput ? config.inputChannels : 0;
    num_out_chans_ = config.outputChannels;

    RtAudio::StreamOptions options{};
    if (config.nonInterleaved)
        options.flags |= RTAUDIO_NONINTERLEAVED;

    unsigned int frames = config.frames;

    RtAudioErrorType e = dac_->openStream(
        &oParams,
        config.enableInput ? &iParams : nullptr,
        FORMAT,
        config.sample_rate,
        &frames,
        &AudioSystem::Callback,
        &rack_,
        &options);

    if (e != RTAUDIO_NO_ERROR) {
        spdlog::error("AudioSystem: openStream failed");
        return false;
    }

    // Feed back the actual negotiated values
    config.frames = frames;

    Audio::frames_     = frames;
    Audio::sample_rate_ = config.sample_rate;

    return true;
}

bool AudioSystem::Start() {
    if (!dac_) return false;
    return dac_->startStream() == RTAUDIO_NO_ERROR;
}

void AudioSystem::Stop() {
    if (dac_ && dac_->isStreamRunning())
        dac_->stopStream();
}

} // namespace augr