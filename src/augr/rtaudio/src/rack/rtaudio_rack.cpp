#include <RtAudio.h>

#include <augr/core/audio.h>
#include <augr/core/rack/module/audio_device.h>

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

    if (graph_dirty_) RebuildExecutionOrder();  // only on topology change
    const Audio input(static_cast<fy_real *>(inbuf), devNumInChans_);
    audio_input_device_->audio_out_->Write(input);

    for (const auto &m : sorted_modules_) {
        m->Process();
    }

    Audio output = audio_output_device_->audio_in_->Read();

    if (output.layout_ != ChannelLayout::kNull) {
        output.WritePlanar(static_cast<fy_buffer_t>(outbuf), SCALE);
    }
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
    audio_dac_ = new RtAudio();

    if (audio_dac().getDeviceCount() < 1) {
        std::cout << "No audio devices found!\n";
        return false;
    }

    CreateAudioInputDevice();
    CreateAudioOutputDevice();

    RtAudio::DeviceInfo info_in =
        audio_dac().getDeviceInfo(audio_dac().getDefaultInputDevice());
    RtAudio::DeviceInfo info_out =
        audio_dac().getDeviceInfo(audio_dac().getDefaultOutputDevice());
    RtAudio::StreamParameters iParams, oParams;

    iParams.deviceId = audio_dac().getDefaultInputDevice();
    devNumInChans_ = info_in.inputChannels;
    iParams.nChannels = devNumInChans_;
    iParams.firstChannel = 0;

    oParams.deviceId = audio_dac().getDefaultOutputDevice();
    devNumOutChans_ = info_out.outputChannels;
    oParams.nChannels = devNumOutChans_;
    oParams.firstChannel = 0;

    RtAudio::StreamOptions options;
    options.flags |= RTAUDIO_NONINTERLEAVED;

    RtAudioErrorType e =
        audio_dac().openStream(&oParams, &iParams, FORMAT, Audio::sampleRate(),
                               &Audio::frames_, AudioCallback, this, &options);

    return e == RtAudioErrorType::RTAUDIO_NO_ERROR;
}

bool RtAudioRack::Start() {
    RtAudioErrorType e = audio_dac().startStream();
    return e == RtAudioErrorType::RTAUDIO_NO_ERROR;
}

void RtAudioRack::Stop() { RtAudioErrorType e = audio_dac().stopStream(); }

} // namespace augr