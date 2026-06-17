#include <augr/audio.h>

#include <augr/exe/rack/audio_system.h>
#include <augr/exe/rack/exe_rack.h>

namespace augr {

namespace {
constexpr RtAudioFormat kFormat = RTAUDIO_FLOAT32;
}

AudioSystem::AudioSystem(ExeRack &rack) : rack_(rack) {}

AudioSystem::~AudioSystem() { Stop(); }

int AudioSystem::Callback(void *outputBuffer, void *inputBuffer,
                          unsigned int nBufferFrames, double streamTime,
                          RtAudioStreamStatus status, void *userdata) {
    return static_cast<ExeRack *>(userdata)->ProcessAudio(
        streamTime, inputBuffer, outputBuffer, nBufferFrames);
}

bool AudioSystem::Configure(AudioConfig &config) {
    // Configure resolves the device IDs, channel counts, sample rate,
    // and frame size. We do not open a stream here — that happens in Start().
    RtAudio probe(RtAudio::Api::UNSPECIFIED,
                  [](RtAudioErrorType type, const std::string &msg) {
                      spdlog::error("RtAudio probe error type={} message={}",
                                    static_cast<int>(type), msg);
                  });

    if (probe.getDeviceCount() < 1) {
        spdlog::error("AudioSystem: no audio devices found");
        return false;
    }

    AudioConfigurator configurator(probe);
    if (!configurator.configure(config)) {
        spdlog::error("AudioSystem: failed to configure audio devices");
        return false;
    }

    config_ = config; // remember the resolved config for Start()
    configured_ = true;
    return true;
}

bool AudioSystem::Start() {
    if (!configured_) {
        spdlog::error(
            "AudioSystem::Start: not configured; call Configure() first");
        return false;
    }
    if (dac_) {
        // Already running. Idempotent.
        return true;
    }

    dac_ = std::make_unique<RtAudio>(
        RtAudio::Api::UNSPECIFIED,
        [](RtAudioErrorType type, const std::string &msg) {
            spdlog::error("RtAudio error type={} message={}",
                          static_cast<int>(type), msg);
        });

    RtAudio::StreamParameters oParams{};
    oParams.deviceId = config_.outputDeviceId;
    oParams.nChannels = config_.outputChannels;
    oParams.firstChannel = 0;

    RtAudio::StreamParameters iParams{};
    if (config_.enableInput) {
        iParams.deviceId = config_.inputDeviceId;
        iParams.nChannels = config_.inputChannels;
        iParams.firstChannel = 0;
    }

    num_in_chans_ = config_.enableInput ? config_.inputChannels : 0;
    num_out_chans_ = config_.outputChannels;

    RtAudio::StreamOptions options{};
    if (config_.nonInterleaved) {
        options.flags |= RTAUDIO_NONINTERLEAVED;
    }

    unsigned int frames = config_.frames;

    auto err = dac_->openStream(
        &oParams, config_.enableInput ? &iParams : nullptr, kFormat,
        config_.sample_rate, &frames, &AudioSystem::Callback, &rack_, &options);

    if (err != RTAUDIO_NO_ERROR) {
        spdlog::error("AudioSystem::Start: openStream failed: {}",
                      dac_->getErrorText());
        dac_.reset();
        return false;
    }

    // Feed back the actual negotiated values.
    config_.frames = frames;
    Audio::set_frames(frames);
    Audio::set_sample_rate(config_.sample_rate);

    err = dac_->startStream();
    if (err != RTAUDIO_NO_ERROR) {
        spdlog::error("AudioSystem::Start: startStream failed: {}",
                      dac_->getErrorText());
        dac_->closeStream();
        dac_.reset();
        return false;
    }

    spdlog::info("AudioSystem: started, sample_rate={} frames={} out_chans={}",
                 config_.sample_rate, frames, num_out_chans_);
    return true;
}

void AudioSystem::Stop() {
    if (!dac_)
        return;

    if (dac_->isStreamRunning()) {
        auto err = dac_->stopStream();
        if (err != RTAUDIO_NO_ERROR) {
            spdlog::error("AudioSystem::Stop: stopStream failed: {}",
                          dac_->getErrorText());
        }
    }
    if (dac_->isStreamOpen()) {
        dac_->closeStream();
    }
    dac_.reset();
}

bool AudioSystem::IsRunning() const { return dac_ && dac_->isStreamRunning(); }

} // namespace augr