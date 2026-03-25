#include <RtAudio.h>

#include <augr/core/audio.h>
#include <augr/core/rack/module/audio_device.h>

#include <augr/core/rack/format/exe/exe_rack.h>

namespace augr {

// REFLECT_REGISTER(ExeRack);

// #define FORMAT RTAUDIO_FLOAT64
#define FORMAT RTAUDIO_FLOAT32
#define SCALE 1.0

static int AudioCallback(void *outputBuffer, void *inputBuffer,
                         unsigned int nBufferFrames, double streamTime,
                         RtAudioStreamStatus status, void *rack) {
    return static_cast<ExeRack *>(rack)->ProcessAudio(
        streamTime, inputBuffer, outputBuffer, nBufferFrames);
}

int ExeRack::ProcessAudio(double streamTime, void *inbuf, void *outbuf,
                          unsigned long frames) {
    const Audio input(static_cast<fy_real *>(inbuf), devNumInChans_);
    audio_input_device_->audio_out_->Write(input);

    for (const auto &m : modules_) {
        m->Process();
    }

    Audio output = audio_output_device_->audio_in_->Read();

    if (output.format_ != Audio::Format::kNull) {
        output.WritePlanar(static_cast<fy_buffer_t>(outbuf), SCALE);
    }
    return 0;
}

bool ExeRack::CreateAudioInputDevice() {
    AudioInputDevice &m = ModelFactoryT<AudioInputDevice>::Make(*this);
    AddChild(m);
    audio_input_device_ = &m;
    return true;
}

bool ExeRack::CreateAudioOutputDevice() {
    AudioOutputDevice &m = ModelFactoryT<AudioOutputDevice>::Make(*this);
    AddChild(m);
    audio_output_device_ = &m;
    return true;
}

bool ExeRack::Create() {
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

    /*
    typedef unsigned int RtAudioStreamFlags;
    static const RtAudioStreamFlags RTAUDIO_NONINTERLEAVED = 0x1;    // Use
    non-interleaved buffers (default = interleaved). static const
    RtAudioStreamFlags RTAUDIO_MINIMIZE_LATENCY = 0x2;  // Attempt to set stream
    parameters for lowest possible latency. static const RtAudioStreamFlags
    RTAUDIO_HOG_DEVICE = 0x4;        // Attempt grab device and prevent use by
    others. static const RtAudioStreamFlags RTAUDIO_SCHEDULE_REALTIME = 0x8; //
    Try to select realtime scheduling for callback thread. static const
    RtAudioStreamFlags RTAUDIO_ALSA_USE_DEFAULT = 0x10; // Use the "default" PCM
    device (ALSA only). static const RtAudioStreamFlags
    RTAUDIO_JACK_DONT_CONNECT = 0x20; // Do not automatically connect ports
    (JACK only).
    */
    RtAudio::StreamOptions options;
    options.flags |= RTAUDIO_NONINTERLEAVED;
    /*
    void openStream( RtAudio::StreamParameters *outputParameters,
                     RtAudio::StreamParameters *inputParameters,
                     RtAudioFormat format, unsigned int sampleRate,
                     unsigned int *bufferFrames, RtAudioCallback callback,
                     void *userData = NULL, RtAudio::StreamOptions *options =
    NULL, RtAudioErrorCallback errorCallback = NULL );
    */
    RtAudioErrorType e =
        audio_dac().openStream(&oParams, &iParams, FORMAT, Audio::sampleRate(),
                               &Audio::frames_, AudioCallback, this, &options);

    return e == RtAudioErrorType::RTAUDIO_NO_ERROR;
}

bool ExeRack::Start() {
    RtAudioErrorType e = audio_dac().startStream();
    return e == RtAudioErrorType::RTAUDIO_NO_ERROR;
}

void ExeRack::Stop() { RtAudioErrorType e = audio_dac().stopStream(); }

} // namespace augr