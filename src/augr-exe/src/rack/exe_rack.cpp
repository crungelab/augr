#include <spdlog/spdlog.h>

#include <augr/core/archiver_factory.h>
#include <augr/core/audio.h>
#include <augr/core/model_factory.h>

#include <augr/rack/archiver/rack_archiver.h>
#include <augr/rack/module/audio_device.h>
#include <augr/rack/module/midi_device.h>

#include <augr/exe/rack/exe_rack.h>

#define SCALE 1.0

namespace augr {

// ---------------------------------------------------------------------------
// Audio thread entry point
// ---------------------------------------------------------------------------

int ExeRack::ProcessAudio(double streamTime, void *inbuf, void *outbuf,
                          unsigned long frames) {

    Process();
    ProcessActions();

    Audio output = audio_output_device_->audio_in_->Read();

    if (output.layout() != ChannelLayout::kNull) {
        output.WritePlanar(static_cast<fy_buffer_t>(outbuf), SCALE);
    } else {
        std::fill_n(static_cast<fy_real *>(outbuf),
                    frames * config_.audio_output_channels, 0.0f);
    }

    ProcessUpdateActions();
    return 0;
}

// ---------------------------------------------------------------------------
// Lifecycle
// ---------------------------------------------------------------------------

void ExeRack::OnCreate() {
    AudioConfig audio_cfg;
    audio_cfg.sample_rate = config_.sample_rate;
    audio_cfg.frames = config_.frames;
    audio_cfg.enableInput = (config_.audio_input_channels > 0);
    audio_system_.Configure(audio_cfg);

    MidiConfig midi_config;
    midi_config.openAllDevicePorts = true;

    midi_system_.Configure(midi_config);

    // Backend may have settled on different values than we requested.
    // Reflect the actual values back into config_ so subsequent
    // AddDefaultDevices uses correct channel counts and rates.
    config_.sample_rate = audio_cfg.sample_rate;
    config_.frames = audio_cfg.frames;
    if (config_.audio_input_channels > 0) {
        config_.audio_input_channels = audio_system_.num_in_chans();
    }
    if (config_.audio_output_channels == 0) {
        config_.audio_output_channels = audio_system_.num_out_chans();
    }

    Rack::OnCreate();
}

bool ExeRack::Start() {
    bool success = Rack::Start();
    audio_system_.Start();
    midi_system_.Start();
    return success;
}

void ExeRack::Stop() {
    Rack::Stop();
    audio_system_.Stop();
    midi_system_.Stop();
}

} // namespace augr

using namespace augr;
DEFINE_MODEL_FACTORY(ExeRack, "Rack", "")

class ExeRackArchiver : public RackArchiver {};
DEFINE_ARCHIVER_FACTORY(ExeRackArchiver, ExeRack, "Rack")
