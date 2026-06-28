#pragma once

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstddef>

#include <augr/audio.h>
#include <augr/ui/ui_builder.h>

#include <augr/rack/pin/audio_pin.h>
#include <augr/rack/module/module.h>
#include <augr/rack/node.h>

namespace augr {

/*
 * Probe module. Tracks the peak level of an incoming audio signal with
 * an exponential decay, like a classic PPM/VU meter. The audio thread
 * updates a running peak each block; the UI thread reads it with a
 * single atomic load — no ring buffer needed since this is a scalar
 * readout, not a waveform capture.
 */

class ProbeModule : public Module {
public:
    void OnCreate() override {
        Module::OnCreate();
        label_ = "Probe";
    }

    void CreateControls() override {
        UiBuilder ui(console_);
        auto decayParam = CreateFloatParameter(
            "Decay", ControlMeta::kDefault,
            &decay_ms_, 300.f, 10.f, 2000.f, 1.f);
        ui.Knob("Decay", decayParam);
    }

    void CreatePins() override {
        audio_in_ = new AudioInput(*this, "audio_in", ChannelLayout::kMono);
        AddInput(*audio_in_);
    }

    void Process() override {
        Audio audio_in = audio_in_->Read();
        const int nFrames = Audio::frames();
        const bool has_audio = audio_in.layout() != ChannelLayout::kNull;

        if (!has_audio) return;

        const fy_real *in = audio_in.array().data();

        // Per-sample decay coefficient derived from the Decay (ms) param.
        // Recomputed once per block (cheap relative to block size), not
        // cached across blocks, since decay_ms_ can be tweaked live.
        const float decay_per_sample =
            std::exp(-1.f / (0.001f * decay_ms_ * kSampleRate));

        float peak = peak_.load(std::memory_order_relaxed);
        for (int i = 0; i < nFrames; ++i) {
            const float v = std::fabs(static_cast<float>(in[i]));
            peak = std::max(v, peak * decay_per_sample);
        }
        peak_.store(peak, std::memory_order_release);
    }

    // UI-thread read path. A single atomic load — no copying or
    // index math needed since there's only one value to report.
    float Snapshot() const {
        return peak_.load(std::memory_order_acquire);
    }

    // Data members
    float decay_ms_ = 300.f;

    REFLECT_ENABLE(Module)

private:
    // TODO: replace with whatever your actual sample-rate accessor is
    // (e.g. Audio::sample_rate()) — Scope didn't need this so I don't
    // have a confirmed name for it from that file.
    static constexpr float kSampleRate = 48000.f;

    AudioInput *audio_in_ = nullptr;

    std::atomic<float> peak_{0.f};
};

} // namespace augr