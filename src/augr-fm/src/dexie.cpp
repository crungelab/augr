// augr/fm/dexie.cpp

#include <algorithm>
#include <cmath>

#include <augr/core/ui/ui_builder.h>
#include <augr/fm/dexie.h>

namespace augr::fm {

namespace {

constexpr float kTwoPi = 6.28318530717958647692f;

float CvToFreq(float cv) { return 261.6255653f * std::pow(2.f, cv); }

// DX7-style rate 0..99 → per-sample increment toward target level.
// ~10 s at rate 0, ~1 ms at rate 99. Simplified — not bit-exact.
float RateToIncrement(float rate_0_99, float sample_rate) {
    const float r = std::clamp(rate_0_99, 0.0f, 99.0f) / 99.0f;
    const float seconds = 10.0f * std::pow(0.0001f, r);
    return 1.0f / (seconds * sample_rate);
}

float LevelToAmplitude(float level_0_99) {
    return std::clamp(level_0_99, 0.0f, 99.0f) / 99.0f;
}

} // namespace

void Dexie::OnCreate() {
    Module::OnCreate();
    label_ = "Dexie";
}

void Dexie::CreateControls() {
    UiBuilder ui(shared_from_this());

    // EG section — matches DX7 operator page ordering
    {
        auto _ = ui.HBox("EG Rates");
        auto r1 = CreateFloatParameter("R1", ControlMeta::kDefault, &rates_[0],
                                       99.f, 0.f, 99.f, 1.f);
        auto r2 = CreateFloatParameter("R2", ControlMeta::kDefault, &rates_[1],
                                       50.f, 0.f, 99.f, 1.f);
        auto r3 = CreateFloatParameter("R3", ControlMeta::kDefault, &rates_[2],
                                       50.f, 0.f, 99.f, 1.f);
        auto r4 = CreateFloatParameter("R4", ControlMeta::kDefault, &rates_[3],
                                       50.f, 0.f, 99.f, 1.f);
        ui.Knob("R1", r1);
        ui.Knob("R2", r2);
        ui.Knob("R3", r3);
        ui.Knob("R4", r4);
    }
    {
        auto _ = ui.HBox("EG Levels");
        auto l1 = CreateFloatParameter("L1", ControlMeta::kDefault, &levels_[0],
                                       99.f, 0.f, 99.f, 1.f);
        auto l2 = CreateFloatParameter("L2", ControlMeta::kDefault, &levels_[1],
                                       75.f, 0.f, 99.f, 1.f);
        auto l3 = CreateFloatParameter("L3", ControlMeta::kDefault, &levels_[2],
                                       50.f, 0.f, 99.f, 1.f);
        auto l4 = CreateFloatParameter("L4", ControlMeta::kDefault, &levels_[3],
                                       0.f, 0.f, 99.f, 1.f);
        ui.Knob("L1", l1);
        ui.Knob("L2", l2);
        ui.Knob("L3", l3);
        ui.Knob("L4", l4);
    }

    // Oscillator section
    {
        auto _ = ui.HBox("Oscillator");
        auto coarse = CreateFloatParameter("Coarse", ControlMeta::kDefault,
                                           &ratio_coarse_, 1.f, 0.f, 16.f, 1.f);
        auto fine = CreateFloatParameter("Fine", ControlMeta::kDefault,
                                         &ratio_fine_, 0.f, 0.f, 0.99f, 0.01f);
        auto detune = CreateFloatParameter("Detune", ControlMeta::kDefault,
                                           &detune_, 0.f, -7.f, 7.f, 1.f);
        auto level = CreateFloatParameter("Level", ControlMeta::kDefault,
                                          &output_level_, 1.f, 0.f, 1.f, 0.01f);
        auto feedback = CreateFloatParameter("Feedback", ControlMeta::kDefault,
                                             &feedback_, 0.f, 0.f, 7.f, 1.f);
        ui.Knob("Coarse", coarse);
        ui.Knob("Fine", fine);
        ui.Knob("Detune", detune);
        ui.Knob("Level", level);
        ui.Knob("Feedback", feedback);
    }
}

void Dexie::CreatePins() {
    cv_pitch_in_ = new VoltageInput(*this, "pitch");
    AddInput(*cv_pitch_in_);
    gate_in_ = new VoltageInput(*this, "gate");
    AddInput(*gate_in_);
    cv_phase_in_ = new VoltageInput(*this, "phase");
    AddInput(*cv_phase_in_);
    audio_out_ = new AudioOutput(*this, "out", ChannelLayout::kMono);
    AddOutput(*audio_out_);
}

void Dexie::Process() {
    const Audio pitch_buf = cv_pitch_in_->Read();
    const Audio gate_buf = gate_in_->Read();
    const Audio phase_buf = cv_phase_in_->Read();

    const float sample_rate = Audio::sample_rate();
    const std::size_t frames = Audio::frames();

    const fy_real *pitch_data =
        pitch_buf.Empty() ? nullptr : pitch_buf.array().data();
    const fy_real *gate_data =
        gate_buf.Empty() ? nullptr : gate_buf.array().data();
    const fy_real *phase_data =
        phase_buf.Empty() ? nullptr : phase_buf.array().data();

    // DX7 detune: ±7 steps → ±3.4 cents total (~0.486 cents/step).
    const float detune_cents = std::round(detune_) * (3.4f / 7.f);
    const float detune_factor = std::pow(2.f, detune_cents / 1200.f);

    const float ratio = ratio_coarse_ + ratio_fine_;
    const bool ratio_mode = ratio > 0.0f;

    // Precompute EG increments — rates don't change mid-buffer.
    const float eg_inc[4] = {
        RateToIncrement(rates_[0], sample_rate),
        RateToIncrement(rates_[1], sample_rate),
        RateToIncrement(rates_[2], sample_rate),
        RateToIncrement(rates_[3], sample_rate),
    };
    const float eg_target[4] = {
        LevelToAmplitude(levels_[0]),
        LevelToAmplitude(levels_[1]),
        LevelToAmplitude(levels_[2]),
        LevelToAmplitude(levels_[3]),
    };

    // Combined feedback depth — per-algorithm scale times the raw 0..7
    // patch amount. The two-sample average (fb_hist_[0]+fb_hist_[1])*0.5
    // is a stability fix (see compute_fb in MSFA's fm_op_kernel.cc),
    // independent of this depth scaling.
    const float feedback_depth = feedback_scale_ * feedback_;

    Audio out(ChannelLayout::kMono);
    fy_real *out_data = out.array().data();

    for (std::size_t i = 0; i < frames; ++i) {
        // --- Envelope ---
        const bool gate = gate_data && (gate_data[i] > 0.5f);
        if (gate && !gate_prev_)
            stage_ = Stage::kR1;
        else if (!gate && gate_prev_)
            stage_ = Stage::kR4;
        gate_prev_ = gate;

        auto advance = [&](int idx, Stage next) {
            const float target = eg_target[idx];
            const float inc = eg_inc[idx];
            if (eg_value_ < target)
                eg_value_ = std::min(eg_value_ + inc, target);
            else if (eg_value_ > target)
                eg_value_ = std::max(eg_value_ - inc, target);
            else
                stage_ = next;
        };

        switch (stage_) {
        case Stage::kIdle:
            break;
        case Stage::kR1:
            advance(0, Stage::kR2);
            break;
        case Stage::kR2:
            advance(1, Stage::kR3);
            break;
        case Stage::kR3:
            advance(2, Stage::kR3);
            break; // sustain
        case Stage::kR4:
            advance(3, Stage::kIdle);
            if (eg_value_ <= 0.0001f) {
                eg_value_ = 0.0f;
                stage_ = Stage::kIdle;
            }
            break;
        }

        // --- Oscillator ---
        const float pitch_cv =
            pitch_data ? static_cast<float>(pitch_data[i]) : 0.0f;
        const float phase_mod =
            phase_data ? static_cast<float>(phase_data[i]) : 0.0f;

        const float base_hz = CvToFreq(pitch_cv) * detune_factor;
        const float freq = ratio_mode ? (base_hz * ratio) : frequency_;
        const float phase_inc = freq / sample_rate;

        const float fb = feedback_depth * (fb_hist_[0] + fb_hist_[1]) * 0.5f;
        const float sample = std::sin(kTwoPi * (phase_ + phase_mod + fb));
        const float shaped = sample * output_level_ * eg_value_;

        out_data[i] = static_cast<fy_real>(shaped);

        fb_hist_[1] = fb_hist_[0];
        fb_hist_[0] = shaped;

        phase_ += phase_inc;
        if (phase_ >= 1.0f)
            phase_ -= std::floor(phase_);
    }

    audio_out_->Write(out);
}

} // namespace augr::fm