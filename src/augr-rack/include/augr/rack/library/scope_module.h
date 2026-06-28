#pragma once

#include <atomic>
#include <array>
#include <cstddef>
#include <cstdint>

#include <augr/audio.h>
#include <augr/ui/ui_builder.h>

#include <augr/rack/pin/audio_pin.h>
#include <augr/rack/module/module.h>
#include <augr/rack/node.h>

namespace augr {

/*
 * Oscilloscope module. Captures an incoming audio signal into a lock-free
 * ring buffer that the UI thread can snapshot for display. The audio thread
 * writes; the UI thread reads the most recent N samples without consuming.
 */

class ScopeModule : public Module {
public:
    static constexpr std::size_t kRingSize = 16384;
    //static constexpr std::size_t kRingSize = 32768;

    void OnCreate() override {
        Module::OnCreate();
        label_ = "Scope";
    }

    void CreatePins() override {
        audio_in_ = new AudioInput(*this, "audio_in", ChannelLayout::kMono);
        AddInput(*audio_in_);
    }

    void CreateControls() override {
        UiBuilder ui(console_);
        auto windowParam = CreateFloatParameter(
            "Window", ControlMeta::kDefault,
            &window_samples_, 1024.f, 128.f, 8192.f, 1.f);
        ui.Knob("Window", windowParam);

        auto triggerParam = CreateFloatParameter(
            "Trigger", ControlMeta::kDefault,
            &trigger_level_, 0.f, -1.f, 1.f, 0.01f);
        ui.Knob("Trigger", triggerParam);
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
    // Returns the number of samples actually written (may be less if the
    // ring hasn't filled yet). Non-consuming — safe to call every frame.
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

    // Data members
    float window_samples_ = 1024.f;
    float trigger_level_ = 0.f;

    REFLECT_ENABLE(Module)

private:
    AudioInput *audio_in_ = nullptr;

    std::array<float, kRingSize> ring_{};
    std::atomic<std::uint64_t> write_{0};
};

} // namespace augr