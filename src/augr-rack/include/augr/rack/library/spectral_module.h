#pragma once

#include <atomic>
#include <array>
#include <cstddef>
#include <cstdint>

#include <augr/core/audio.h>
#include <augr/core/ui/ui_builder.h>

#include <augr/rack/audio_pin.h>
#include <augr/rack/module/module.h>
#include <augr/rack/node.h>

namespace augr {

/*
 * Spectrum Analyzer module. Continuously captures an incoming audio signal
 * into a lock-free ring buffer. The UI thread snapshots the most recent
 * FFT-sized window of samples for frequency-domain display.
 */

class SpectralModule : public Module {
public:
    static constexpr std::size_t kRingSize = 16384;

    void OnCreate() override {
        Module::OnCreate();
        label_ = "Spectral";

        sample_rate_ = static_cast<float>(Audio::sample_rate());
    }

    void CreateControls() override {
        UiBuilder ui(shared_from_this());

        auto smoothParam = CreateFloatParameter(
            "Smooth", ControlMeta::kDefault,
            &smoothing_, 0.6f, 0.f, 0.99f, 0.01f);
        ui.Knob("Smooth", smoothParam);

        auto floorParam = CreateFloatParameter(
            "Floor", ControlMeta::kDefault,
            &floor_db_, -80.f, -120.f, -40.f, 1.f);
        ui.Knob("Floor", floorParam);
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
        const auto w = write_.load(std::memory_order_relaxed);
        for (int i = 0; i < nFrames; ++i) {
            ring_[(w + i) % kRingSize] = static_cast<float>(in[i]);
        }
        write_.store(w + nFrames, std::memory_order_release);
    }

    // UI-thread read path. Copies the most recent `count` samples into `out`.
    // Non-consuming — safe to call every UI frame. Same pattern as ScopeModule.
    std::size_t Snapshot(float *out, std::size_t count) const {
        const auto w = write_.load(std::memory_order_acquire);
        const std::size_t available =
            static_cast<std::size_t>(std::min<std::uint64_t>(w, kRingSize));
        const std::size_t to_read = std::min(available, count);
        const std::uint64_t start = w - to_read;
        for (std::size_t i = 0; i < to_read; ++i) {
            out[i] = ring_[(start + i) % kRingSize];
        }
        return to_read;
    }

    float SampleRate() const { return sample_rate_; }

    // Data members
    float smoothing_ = 0.6f;   // UI-side temporal smoothing α for magnitudes
    float floor_db_ = -80.f;   // lower bound of the dB display range

    REFLECT_ENABLE(Module)

private:
    AudioInput *audio_in_ = nullptr;

    std::array<float, kRingSize> ring_{};
    std::atomic<std::uint64_t> write_{0};
    float sample_rate_ = 48000.f;
};

} // namespace augr