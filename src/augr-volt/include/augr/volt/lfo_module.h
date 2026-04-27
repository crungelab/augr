#pragma once

#include <cmath>
#include <cstdint>
#include <vector>

#include <augr/core/audio.h>
#include <augr/core/ui/ui_builder.h>

#include <augr/rack/module/module.h>
#include <augr/rack/node.h>
#include <augr/rack/voltage_pin.h>

namespace augr {

class LfoModule : public Module {
public:
    enum class Waveform { Sine, Tri, Square, SawUp, SawDown, SampleHold };

    bool Create(Part &owner) override {
        Module::Create(owner);
        label_ = "LFO";
        return true;
    }

    void CreatePins() override {
        cv_rate_in_ = new VoltageInput(*this, "cv_rate_in");
        AddInput(*cv_rate_in_);

        reset_in_ = new VoltageInput(*this, "reset_in");
        AddInput(*reset_in_);

        cv_out_ = new VoltageOutput(*this, "cv_out");
        AddOutput(*cv_out_);
    }

    void CreateControls() override {
        UiBuilder ui(*this);

        auto rateParam = CreateFloatParameter(
            "Rate", ControlMeta::kDefault, &rate_, 0.f, -6.64f, 4.32f, 0.01f);
        ui.Knob("Rate", rateParam);

        std::vector<EnumParameterT<Waveform>::Choice> waveformChoices = {
            {Waveform::Sine, "Sine"},        {Waveform::Tri, "Tri"},
            {Waveform::Square, "Square"},    {Waveform::SawUp, "Saw Up"},
            {Waveform::SawDown, "Saw Down"}, {Waveform::SampleHold, "S&H"},
        };
        auto waveformParam =
            CreateEnumParameter("Waveform", ControlMeta::kDefault, &waveform_,
                                waveformChoices, Waveform::Sine);
        ui.Combo("Waveform", waveformParam);
    }

    void Process() override {
        Audio rate_cv = cv_rate_in_->Read();
        Audio reset_cv = reset_in_->Read();
        Audio output(ChannelLayout::kMono);

        const int nFrames = Audio::frames();
        const float sr = Audio::sample_rate();
        const bool has_rate_cv = rate_cv.layout_ != ChannelLayout::kNull;
        const bool has_reset = reset_cv.layout_ != ChannelLayout::kNull;

        fy_real *out = output.array().data();
        const fy_real *rate_buf =
            has_rate_cv ? rate_cv.array().data() : nullptr;
        const fy_real *reset_buf =
            has_reset ? reset_cv.array().data() : nullptr;

        for (int i = 0; i < nFrames; ++i) {
            // Reset edge detection.
            const bool reset_high = has_reset && reset_buf[i] >= kGateThreshold;
            if (reset_high && !reset_was_high_) {
                phase_ = 0.f;
                // Also resample S&H so the reset is audible.
                sh_value_ = NextRandom();
            }
            reset_was_high_ = reset_high;

            // Rate in octaves above 1 Hz, plus CV (1V/oct).
            const float rate_oct =
                rate_ + (has_rate_cv ? float(rate_buf[i]) : 0.f);
            const float freq = std::pow(2.f, rate_oct);
            const float dt = freq / sr;

            out[i] = tick(phase_);

            // Advance phase and detect wrap for S&H resampling.
            phase_ += dt;
            if (phase_ >= 1.f) {
                phase_ -= 1.f;
                sh_value_ = NextRandom();
            }
        }

        cv_out_->Write(output);
    }

    // Data members
    float rate_ = 0.f; // octaves above 1 Hz
    Waveform waveform_ = Waveform::Sine;

    REFLECT_ENABLE(Module)

private:
    static constexpr float kGateThreshold = 0.5f;

    VoltageInput *cv_rate_in_ = nullptr;
    VoltageInput *reset_in_ = nullptr;
    VoltageOutput *cv_out_ = nullptr;

    float phase_ = 0.f;
    float sh_value_ = 0.f;
    bool reset_was_high_ = false;
    uint32_t rng_state_ = 0x9E3779B9u; // xorshift seed

    float tick(float phase) {
        switch (waveform_) {
        case Waveform::Sine:
            return std::sin(phase * 2.f * float(M_PI));
        case Waveform::Tri:
            // Bipolar triangle: -1 at phase 0, +1 at 0.5, -1 at 1.
            return 4.f * std::abs(phase - 0.5f) - 1.f;
        case Waveform::Square:
            return phase < 0.5f ? 1.f : -1.f;
        case Waveform::SawUp:
            return 2.f * phase - 1.f;
        case Waveform::SawDown:
            return 1.f - 2.f * phase;
        case Waveform::SampleHold:
            return sh_value_;
        }
        return 0.f;
    }

    // Fast xorshift32 RNG producing values in [-1, 1].
    float NextRandom() {
        rng_state_ ^= rng_state_ << 13;
        rng_state_ ^= rng_state_ >> 17;
        rng_state_ ^= rng_state_ << 5;
        // Map uint32 to [-1, 1].
        return (float(rng_state_) / float(UINT32_MAX)) * 2.f - 1.f;
    }
};

} // namespace augr