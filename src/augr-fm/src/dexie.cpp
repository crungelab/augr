// augr/fm/dexie.cpp

#include <algorithm>
#include <cmath>

#include <augr/fm/dexie.h>
#include <augr/ui/ui_builder.h>

namespace augr::fm {

namespace {

constexpr float kTwoPi = 6.28318530717958647692f;

float CvToFreq(float cv) { return 261.6255653f * std::pow(2.f, cv); }

constexpr int kFeedbackBitdepth = 8;

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
                                          &output_level_, 99.f, 0.f, 99.f, 1.f);
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
    if (muted_) {
        audio_out_->Write(Audio()); // write silence
        return;
    }

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

    env_.SetSampleRate(sample_rate);

    // DX7 detune: ±7 steps → ±3.4 cents total (~0.486 cents/step).
    const float detune_cents = std::round(detune_) * (3.4f / 7.f);
    const float detune_factor = std::pow(2.f, detune_cents / 1200.f);

    const float ratio = ratio_coarse_ + ratio_fine_;
    // const float ratio = ratio_coarse_ * (1.0f + ratio_fine_);
    const bool ratio_mode = ratio > 0.0f;

    // Feedback amount 0..7 → shift (FEEDBACK_BITDEPTH - amount), or 16 = off.
    const int feedback_int = static_cast<int>(std::round(feedback_));
    const int feedback_shift =
        feedback_int != 0 ? kFeedbackBitdepth - feedback_int : 16;
    const bool fb_on = feedback_shift < 16;
    const float fb_divisor = static_cast<float>(1 << (feedback_shift + 1));

    Audio out(ChannelLayout::kMono);
    fy_real *out_data = out.array().data();

    for (std::size_t i = 0; i < frames; ++i) {
        // --- Envelope ---
        const bool gate = gate_data && (gate_data[i] > 0.5f);
        if (gate && !gate_prev_)
            env_.NoteOn(rates_, levels_, output_level_, /*rate_scaling=*/0);
        else if (!gate && gate_prev_)
            env_.NoteOff();
        gate_prev_ = gate;

        const float env_amp = env_.Tick();

        // --- Oscillator ---
        const float pitch_cv =
            pitch_data ? static_cast<float>(pitch_data[i]) : 0.0f;
        const float phase_mod =
            phase_data ? static_cast<float>(phase_data[i]) : 0.0f;
        phase_mod_peak_ = std::max(phase_mod_peak_, std::fabs(phase_mod));

        const float base_hz = CvToFreq(pitch_cv) * detune_factor;

        const float freq = ratio_mode ? (base_hz * ratio) : frequency_;
        debug_freq_ = freq; // <-- add this

        const float phase_inc = freq / sample_rate;

        const float fb =
            fb_on ? (fb_hist_[0] + fb_hist_[1]) / fb_divisor : 0.0f;

        const float sample = std::sin(kTwoPi * (phase_ + phase_mod + fb));
        const float shaped =
            sample * env_amp; // output level is baked into env_amp

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