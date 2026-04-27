// augr/fm/envelope.cpp
#include <augr/fm/envelope.h>

#include <algorithm>
#include <cmath>

#include <augr/core/ui/ui_builder.h>

namespace augr::fm {

namespace {

// Convert DX-style 0..99 rate to a per-sample increment toward the target.
// The curve is exponential-ish: higher rates = dramatically faster.
// This is a simplified approximation, not a bit-exact DX7 emulation.
float RateToIncrement(float rate_0_99, float sample_rate) {
    const float r = std::clamp(rate_0_99, 0.0f, 99.0f) / 99.0f;
    // ~10 seconds at rate 0, ~1ms at rate 99
    const float seconds = 10.0f * std::pow(0.0001f, r);
    return 1.0f / (seconds * sample_rate);
}

float LevelToAmplitude(float level_0_99) {
    return std::clamp(level_0_99, 0.0f, 99.0f) / 99.0f;
}

} // namespace

bool Envelope::Create(Part &owner) {
    if (!Module::Create(owner))
        return false;
    label_ = "Envelope";
    return true;
}

void Envelope::CreateControls() {
    UiBuilder ui(*this);

    auto r1 = CreateFloatParameter("R1", ControlMeta::kDefault, &rates_[0],  99.f, 0.f, 99.f, 1.f);
    auto r2 = CreateFloatParameter("R2", ControlMeta::kDefault, &rates_[1],  50.f, 0.f, 99.f, 1.f);
    auto r3 = CreateFloatParameter("R3", ControlMeta::kDefault, &rates_[2],  50.f, 0.f, 99.f, 1.f);
    auto r4 = CreateFloatParameter("R4", ControlMeta::kDefault, &rates_[3],  50.f, 0.f, 99.f, 1.f);
    ui.Knob("R1", r1); ui.Knob("R2", r2); ui.Knob("R3", r3); ui.Knob("R4", r4);

    auto l1 = CreateFloatParameter("L1", ControlMeta::kDefault, &levels_[0], 99.f, 0.f, 99.f, 1.f);
    auto l2 = CreateFloatParameter("L2", ControlMeta::kDefault, &levels_[1], 75.f, 0.f, 99.f, 1.f);
    auto l3 = CreateFloatParameter("L3", ControlMeta::kDefault, &levels_[2], 50.f, 0.f, 99.f, 1.f);
    auto l4 = CreateFloatParameter("L4", ControlMeta::kDefault, &levels_[3],  0.f, 0.f, 99.f, 1.f);
    ui.Knob("L1", l1); ui.Knob("L2", l2); ui.Knob("L3", l3); ui.Knob("L4", l4);
}

void Envelope::CreatePins() {
    gate_in_ = new VoltageInput(*this, "gate");
    AddInput(*gate_in_);
    cv_out_ = new VoltageOutput(*this, "cv");
    AddOutput(*cv_out_);
}

void Envelope::Process() {
    const Audio gate_buf = gate_in_->Read();
    const float sample_rate = Audio::sample_rate();
    const std::size_t frames = Audio::frames();

    const fy_real *gate_data = gate_buf.Empty() ? nullptr : gate_buf.array().data();

    Audio out(ChannelLayout::kMono);
    fy_real *out_data = out.array().data();

    for (std::size_t i = 0; i < frames; ++i) {
        const bool gate = gate_data && (gate_data[i] > 0.5f);

        // Gate transitions drive stage changes.
        if (gate && !gate_prev_) {
            stage_ = Stage::kR1;
        } else if (!gate && gate_prev_) {
            stage_ = Stage::kR4;
        }
        gate_prev_ = gate;

        // Advance the current stage toward its target level.
        auto advance = [&](int idx, Stage next) {
            const float target = LevelToAmplitude(levels_[idx]);
            const float inc = RateToIncrement(rates_[idx], sample_rate);
            if (value_ < target)
                value_ = std::min(value_ + inc, target);
            else if (value_ > target)
                value_ = std::max(value_ - inc, target);
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
            if (value_ <= 0.0001f) {
                value_ = 0.0f;
                stage_ = Stage::kIdle;
            }
            break;
        }

        out_data[i] = static_cast<fy_real>(value_);
    }

    cv_out_->Write(out);
}

} // namespace augr::fm
