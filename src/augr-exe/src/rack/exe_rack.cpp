#include <augr/exe/rack/exe_rack.h>
#include <augr/core/audio.h>
#include <augr/core/rack/module/audio_device.h>
#include <spdlog/spdlog.h>

#define SCALE 1.0

namespace augr {

// ---------------------------------------------------------------------------
// Audio thread entry point
// ---------------------------------------------------------------------------

int ExeRack::ProcessAudio(double streamTime, void *inbuf, void *outbuf,
                          unsigned long frames) {
    if (graph_dirty_)
        RebuildExecutionOrder();

    ProcessActions();

    for (const auto &m : sorted_modules_)
        m->Process();

    Audio output = audio_output_device_->audio_in_->Read();

    if (output.layout_ != ChannelLayout::kNull) {
        output.WritePlanar(static_cast<fy_buffer_t>(outbuf), SCALE);
    } else {
        std::fill_n(static_cast<fy_real *>(outbuf),
                    frames * devNumOutChans_, 0.0f);
    }

    ProcessUpdateActions();
    return 0;
}

// ---------------------------------------------------------------------------
// MIDI thread entry point — just enqueue; processed on the audio thread
// ---------------------------------------------------------------------------

void ExeRack::EnqueueMidiMessage(double timestamp,
                                 const std::vector<unsigned char> &bytes) {
    EnqueueAction([this, timestamp, bytes]() {
        // TODO: route to a MidiInputDevice module, same pattern as audio
        // e.g. midi_input_device_->HandleMessage(timestamp, bytes);
        (void)timestamp;
        (void)bytes;
    });
}

// ---------------------------------------------------------------------------
// Device module helpers
// ---------------------------------------------------------------------------

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

// ---------------------------------------------------------------------------
// Lifecycle
// ---------------------------------------------------------------------------

bool ExeRack::Create() {
    AudioConfig config;
    config.enableInput = false;
    config.sampleRate  = 48000;
    config.frames      = 512;

    if (!audio_system_.Create(config))
        return false;

    devNumInChans_  = audio_system_.num_in_chans();
    devNumOutChans_ = audio_system_.num_out_chans();

    if (config.enableInput)
        CreateAudioInputDevice();
    CreateAudioOutputDevice();

    // MIDI is best-effort; don't fail the rack if no ports exist
    midi_system_.Create();

    return true;
}

bool ExeRack::Start() {
    return audio_system_.Start();
}

void ExeRack::Stop() {
    audio_system_.Stop();
    midi_system_.Stop();
}

} // namespace augr