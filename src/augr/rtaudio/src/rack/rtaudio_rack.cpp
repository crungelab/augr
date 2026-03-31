#include <RtAudio.h>

#include <augr/core/audio.h>
#include <augr/core/rack/module/audio_device.h>

#include <augr/rtaudio/rack/audio_configurator.h>
#include <augr/rtaudio/rack/rtaudio_rack.h>

namespace augr {

// #define FORMAT RTAUDIO_FLOAT64
#define FORMAT RTAUDIO_FLOAT32
#define SCALE 1.0

static int AudioCallback(void *outputBuffer, void *inputBuffer,
                         unsigned int nBufferFrames, double streamTime,
                         RtAudioStreamStatus status, void *rack) {
    return static_cast<RtAudioRack *>(rack)->ProcessAudio(
        streamTime, inputBuffer, outputBuffer, nBufferFrames);
}

int RtAudioRack::ProcessAudio(double streamTime, void *inbuf, void *outbuf,
                              unsigned long frames) {

    if (graph_dirty_)
        RebuildExecutionOrder(); // only on topology change

    ProcessActions();

    // const Audio input(static_cast<fy_real *>(inbuf), devNumInChans_);
    // audio_input_device_->audio_out_->Write(input);

    for (const auto &m : sorted_modules_) {
        m->Process();
    }

    Audio output = audio_output_device_->audio_in_->Read();

    if (output.layout_ != ChannelLayout::kNull) {
        output.WritePlanar(static_cast<fy_buffer_t>(outbuf), SCALE);
    } else {
        // If no output, write silence
        std::fill_n(static_cast<fy_real *>(outbuf), frames * devNumOutChans_,
                    0.0f);
    }

    ProcessUpdateActions();

    return 0;
}

bool RtAudioRack::CreateAudioInputDevice() {
    AudioInputDevice &m = ModelFactoryT<AudioInputDevice>::Make(*this);
    AddChild(m);
    audio_input_device_ = &m;
    return true;
}

bool RtAudioRack::CreateAudioOutputDevice() {
    AudioOutputDevice &m = ModelFactoryT<AudioOutputDevice>::Make(*this);
    AddChild(m);
    audio_output_device_ = &m;
    return true;
}

bool RtAudioRack::Create() {
    // audio_dac_ = new RtAudio();
    audio_dac_ =
        new RtAudio(RtAudio::Api::UNSPECIFIED,
                    [](RtAudioErrorType type, const std::string &errorText) {
                        spdlog::error("RtAudio error type={} message={}",
                                      static_cast<int>(type), errorText);
                    });

    if (audio_dac().getDeviceCount() < 1) {
        std::cout << "No audio devices found!\n";
        return false;
    }

    AudioConfig config;
    config.enableInput = false; // set true for duplex
    config.sampleRate = 48000;  // preferred target
    config.frames = 512;        // requested buffer size

    AudioConfigurator configurator(audio_dac());
    if (!configurator.configure(config)) {
        std::cout << "Failed to configure audio devices\n";
        return false;
    }

    if (config.enableInput) {
        CreateAudioInputDevice();
    }
    CreateAudioOutputDevice();

    RtAudio::StreamParameters oParams{};
    oParams.deviceId = config.outputDeviceId;
    oParams.nChannels = config.outputChannels;
    oParams.firstChannel = 0;

    RtAudio::StreamParameters iParams{};
    if (config.enableInput) {
        iParams.deviceId = config.inputDeviceId;
        iParams.nChannels = config.inputChannels;
        iParams.firstChannel = 0;
    }

    devNumInChans_ = config.enableInput ? config.inputChannels : 0;
    devNumOutChans_ = config.outputChannels;

    RtAudio::StreamOptions options{};
    if (config.nonInterleaved) {
        options.flags |= RTAUDIO_NONINTERLEAVED;
    }

    unsigned int frames = config.frames;

    RtAudioErrorType e = audio_dac().openStream(
        &oParams, config.enableInput ? &iParams : nullptr, FORMAT,
        config.sampleRate, &frames, AudioCallback, this, &options);

    if (e != RTAUDIO_NO_ERROR) {
        spdlog::error("RtAudio openStream failed");
        return false;
    }

    Audio::frames_ = frames;
    Audio::sampleRate_ = config.sampleRate;
    /*
    RtAudioErrorType e =
        audio_dac().openStream(&oParams, nullptr, FORMAT, Audio::sampleRate(),
                               &Audio::frames_, AudioCallback, this, &options);
    */

    return e == RtAudioErrorType::RTAUDIO_NO_ERROR;
}

bool RtAudioRack::Start() {
    RtAudioErrorType e = audio_dac().startStream();
    return e == RtAudioErrorType::RTAUDIO_NO_ERROR;
}

void RtAudioRack::Stop() { RtAudioErrorType e = audio_dac().stopStream(); }

} // namespace augr