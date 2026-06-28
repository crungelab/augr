// augr/fm/operator.cpp

#include <cmath>

#include <augr/ui/ui_builder.h>
#include <augr/fm/operator.h>

namespace augr::fm {

namespace {
constexpr float kTwoPi = 6.28318530717958647692f;

float cvToFreq(float cv) { return 261.6255653f * std::pow(2.f, cv); }
} // namespace

void Operator::OnCreate() {
    Module::OnCreate();
    label_ = "Operator";
}

void Operator::CreateControls() {
    UiBuilder ui(console_);

    auto _ = ui.HBox("Parameters");

    // DX7-style detune: integer steps -7..7, each step ≈ 0.49 cents.
    auto detuneParam = CreateFloatParameter("Detune", ControlMeta::kDefault,
                                            &detune_, 0.f, -7.f, 7.f, 1.f);
    ui.Knob("Detune", detuneParam);

    auto coarseParam = CreateFloatParameter(
        "Coarse", ControlMeta::kDefault, &ratio_coarse_, 1.f, 0.f, 16.f, 1.f);
    ui.Knob("Coarse", coarseParam);

    auto fineParam = CreateFloatParameter("Fine", ControlMeta::kDefault,
                                          &ratio_fine_, 0.f, 0.f, 0.99f, 0.01f);
    ui.Knob("Fine", fineParam);

    auto levelParam = CreateFloatParameter(
        "Level", ControlMeta::kDefault, &output_level_, 1.f, 0.f, 1.f, 0.01f);
    ui.Knob("Level", levelParam);

    auto feedbackParam = CreateFloatParameter("Feedback", ControlMeta::kDefault,
                                              &feedback_, 0.f, 0.f, 1.f, 0.01f);
    ui.Knob("Feedback", feedbackParam);
}

void Operator::CreatePins() {
    cv_pitch_in_ = new VoltageInput(*this, "pitch");
    AddInput(*cv_pitch_in_);
    cv_level_in_ = new VoltageInput(*this, "level");
    AddInput(*cv_level_in_);
    cv_phase_in_ = new VoltageInput(*this, "phase");
    AddInput(*cv_phase_in_);
    audio_out_ = new AudioOutput(*this, "out", ChannelLayout::kMono);
    AddOutput(*audio_out_);
}

void Operator::Process() {
    const Audio pitch_buf = cv_pitch_in_->Read();
    const Audio level_buf = cv_level_in_->Read();
    const Audio phase_buf = cv_phase_in_->Read();

    const float sample_rate = Audio::sample_rate();
    const std::size_t frames = Audio::frames();

    const fy_real *pitch_data =
        pitch_buf.Empty() ? nullptr : pitch_buf.array().data();
    const fy_real *level_data =
        level_buf.Empty() ? nullptr : level_buf.array().data();
    const fy_real *phase_data =
        phase_buf.Empty() ? nullptr : phase_buf.array().data();

    // DX7 detune: ±7 steps maps to ±3.4 cents total, so each step ≈ 0.4857
    // cents. std::round snaps the float zone value to the nearest integer step.
    const float detune_cents = std::round(detune_) * (3.4f / 7.f);
    const float detune_factor = std::pow(2.f, detune_cents / 1200.f);

    const float ratio = ratio_coarse_ + ratio_fine_;
    const bool ratio_mode = ratio > 0.0f;

    Audio out(ChannelLayout::kMono);
    fy_real *out_data = out.array().data();

    for (std::size_t i = 0; i < frames; ++i) {
        const float pitch_cv =
            pitch_data ? static_cast<float>(pitch_data[i]) : 0.0f;
        const float level =
            level_data ? static_cast<float>(level_data[i]) : 1.0f;
        const float phase_mod =
            phase_data ? static_cast<float>(phase_data[i]) : 0.0f;

        const float base_hz = cvToFreq(pitch_cv) * detune_factor;
        const float freq = ratio_mode ? (base_hz * ratio) : frequency_;
        const float phase_inc = freq / sample_rate;

        const float fb = feedback_ * last_sample_ * 0.5f;

        const float sample = std::sin(kTwoPi * (phase_ + phase_mod + fb));
        const float shaped = sample * output_level_ * level;

        out_data[i] = static_cast<fy_real>(shaped);
        last_sample_ = shaped;

        phase_ += phase_inc;
        if (phase_ >= 1.0f)
            phase_ -= std::floor(phase_);
    }

    audio_out_->Write(out);
}

} // namespace augr::fm