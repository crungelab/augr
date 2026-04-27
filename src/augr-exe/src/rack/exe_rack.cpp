#include <augr/core/audio.h>
#include <augr/rack/module/audio_device.h>
#include <augr/rack/module/midi_device.h>
#include <augr/exe/rack/exe_rack.h>
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

    if (output.layout() != ChannelLayout::kNull) {
        output.WritePlanar(static_cast<fy_buffer_t>(outbuf), SCALE);
    } else {
        std::fill_n(static_cast<fy_real *>(outbuf), frames * devNumOutChans_,
                    0.0f);
    }

    ProcessUpdateActions();
    return 0;
}

// ---------------------------------------------------------------------------
// MIDI thread entry point — just enqueue; processed on the audio thread
// ---------------------------------------------------------------------------

void ExeRack::EnqueueMidiMessage(MidiMessage message) {
    EnqueueAction([this, message = std::move(message)]() {
        // TODO: route to MidiInputDevice's MidiOutput pin
        // e.g. midi_input_device_->midi_out_->Write(message);
        //(void)message;
        midi_input_device_->midi_out_->Write(message);
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

bool ExeRack::CreateMidiInputDevice() {
    MidiInputDevice &m = ModelFactoryT<MidiInputDevice>::Make(*this);
    AddChild(m);
    midi_input_device_ = &m;
    return true;
}

bool ExeRack::CreateMidiOutputDevice() {
    MidiOutputDevice &m = ModelFactoryT<MidiOutputDevice>::Make(*this);
    AddChild(m);
    midi_output_device_ = &m;
    return true;
}

// ---------------------------------------------------------------------------
// Lifecycle
// ---------------------------------------------------------------------------

bool ExeRack::Create() {
    AudioConfig config;
    config.enableInput = false;
    config.sample_rate = 48000;
    config.frames = 512;

    if (!audio_system_.Create(config))
        return false;

    devNumInChans_ = audio_system_.num_in_chans();
    devNumOutChans_ = audio_system_.num_out_chans();

    if (config.enableInput)
        CreateAudioInputDevice();
    CreateAudioOutputDevice();

    // MIDI is best-effort; don't fail the rack if no ports exist
    midi_system_.Create();

    CreateMidiInputDevice();
    //CreateMidiOutputDevice();

    return true;
}

bool ExeRack::Start() { return audio_system_.Start(); }

void ExeRack::Stop() {
    audio_system_.Stop();
    midi_system_.Stop();
}

} // namespace augr