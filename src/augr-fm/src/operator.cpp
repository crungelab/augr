// augr/fm/operator.cpp

#include <cmath>

#include <augr/fm/operator.h>

namespace augr::fm {

namespace {
constexpr float kTwoPi = 6.28318530717958647692f;
}

bool Operator::Create(Part &owner) {
    if (!Module::Create(owner))
        return false;

    cv_pitch_in_ = new VoltageInput(*this, "pitch");
    AddInput(*cv_pitch_in_);
    cv_level_in_ = new VoltageInput(*this, "level");
    AddInput(*cv_level_in_);
    cv_phase_in_ = new VoltageInput(*this, "phase");
    AddInput(*cv_phase_in_);
    audio_out_ = new AudioOutput(*this, "out", ChannelLayout::kMono);
    AddOutput(*audio_out_);
    return true;
}

void Operator::Process() {
    const Audio pitch_buf = cv_pitch_in_->Read();
    const Audio level_buf = cv_level_in_->Read();
    const Audio phase_buf = cv_phase_in_->Read();

    const float sample_rate = Audio::sample_rate();
    const std::size_t frames = Audio::frames();

    const fy_real *pitch_data = pitch_buf.Empty() ? nullptr : pitch_buf.array().data();
    const fy_real *level_data = level_buf.Empty() ? nullptr : level_buf.array().data();
    const fy_real *phase_data = phase_buf.Empty() ? nullptr : phase_buf.array().data();

    const bool ratio_mode = ratio_ > 0.0f;

    Audio out(ChannelLayout::kMono);
    fy_real *out_data = out.array().data();

    for (std::size_t i = 0; i < frames; ++i) {
        const float pitch_hz  = pitch_data ? static_cast<float>(pitch_data[i]) : 0.0f;
        const float level     = level_data ? static_cast<float>(level_data[i]) : 1.0f;
        const float phase_mod = phase_data ? static_cast<float>(phase_data[i]) : 0.0f;

        const float freq = ratio_mode ? (pitch_hz * ratio_) : frequency_;
        const float phase_inc = freq / sample_rate;

        // Feedback: self-modulate using last sample. The 0.5 scaling matches
        // DX7 feedback depth at full level; tune if you want a different curve.
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
