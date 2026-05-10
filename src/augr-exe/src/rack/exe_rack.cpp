#include <spdlog/spdlog.h>

#include <augr/core/audio.h>
#include <augr/core/model_factory.h>

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
    if (graph_dirty_)
        RebuildExecutionOrder();

    ProcessActions();

    for (const auto &m : sorted_modules_)
        m->Process();

    Audio output = audio_output_device_->audio_in_->Read();

    if (output.layout() != ChannelLayout::kNull) {
        output.WritePlanar(static_cast<fy_buffer_t>(outbuf), SCALE);
    } else {
        std::fill_n(static_cast<fy_real *>(outbuf), frames * config_.audio_output_channels,
                    0.0f);
    }

    ProcessUpdateActions();
    return 0;
}

// ---------------------------------------------------------------------------
// Lifecycle
// ---------------------------------------------------------------------------

void ExeRack::Create(Part *owner) {
    AudioConfig audio_cfg;
    audio_cfg.sample_rate = config_.sample_rate;
    audio_cfg.frames = config_.frames;
    audio_cfg.enableInput = (config_.audio_input_channels > 0);
    audio_system_.Create(audio_cfg);

    midi_system_.Create();

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

    Rack::Create(owner);
}

bool ExeRack::Start() { return audio_system_.Start(); }

void ExeRack::Stop() {
    audio_system_.Stop();
    midi_system_.Stop();
}

} // namespace augr