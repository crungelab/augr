#pragma once

#include <algorithm>

#include <augr/core/audio.h>
#include <augr/core/ui/ui_builder.h>

#include <augr/rack/module/module.h>
#include <augr/rack/node.h>
#include <augr/rack/voltage_pin.h>

namespace augr {

class AdsrModule : public Module {
public:
    enum class Stage { Idle, Attack, Decay, Sustain, Release };

    bool Create(Part &owner) override {
        Module::Create(owner);
        label_ = "ADSR";

        gate_in_ = new VoltageInput(*this, "gate_in");
        AddInput(*gate_in_);

        env_out_ = new VoltageOutput(*this, "env_out");
        AddOutput(*env_out_);

        UiBuilder ui(*this);
        auto attackParam =
            CreateFloatParameter("Attack", ControlMeta::kSeconds, &attack_,
                                 0.01f, 0.001f, 10.f, 0.001f);
        ui.Knob("Attack", attackParam);

        auto decayParam =
            CreateFloatParameter("Decay", ControlMeta::kSeconds, &decay_, 0.1f,
                                 0.001f, 10.f, 0.001f);
        ui.Knob("Decay", decayParam);

        auto sustainParam = CreateFloatParameter(
            "Sustain", ControlMeta::kDefault, &sustain_, 0.7f, 0.f, 1.f, 0.01f);
        ui.Knob("Sustain", sustainParam);

        auto releaseParam =
            CreateFloatParameter("Release", ControlMeta::kSeconds, &release_,
                                 0.2f, 0.001f, 10.f, 0.001f);
        ui.Knob("Release", releaseParam);

        return true;
    }

    void Process() override {
        Audio gate_cv = gate_in_->Read();
        Audio output(ChannelLayout::kMono);

        const float sr = Audio::sampleRate();
        const int nFrames = Audio::frames();
        const bool has_gate = gate_cv.layout_ != ChannelLayout::kNull;

        fy_real *out = output.array().data();
        const fy_real *gate = has_gate ? gate_cv.array().data() : nullptr;

        // Precompute rates once per buffer. Knob values are assumed stable
        // within a buffer.
        const float attack_rate = 1.f / (attack_ * sr);
        const float decay_rate = (1.f - sustain_) / (decay_ * sr);
        // release_rate is computed at the moment release begins, since it
        // depends on current level.

        for (int i = 0; i < nFrames; ++i) {
            const bool gate_high = has_gate && gate[i] >= kGateThreshold;

            // Edge detection.
            if (gate_high && !gate_was_high_) {
                // Rising edge: start attack from current level
                // (legato-friendly).
                stage_ = Stage::Attack;
            } else if (!gate_high && gate_was_high_) {
                // Falling edge: start release from current level.
                stage_ = Stage::Release;
                release_rate_ = level_ / (release_ * sr);
            }
            gate_was_high_ = gate_high;

            // Advance envelope based on current stage.
            switch (stage_) {
            case Stage::Attack:
                level_ += attack_rate;
                if (level_ >= 1.f) {
                    level_ = 1.f;
                    stage_ = Stage::Decay;
                }
                break;
            case Stage::Decay:
                level_ -= decay_rate;
                if (level_ <= sustain_) {
                    level_ = sustain_;
                    stage_ = Stage::Sustain;
                }
                break;
            case Stage::Sustain:
                level_ = sustain_;
                break;
            case Stage::Release:
                level_ -= release_rate_;
                if (level_ <= 0.f) {
                    level_ = 0.f;
                    stage_ = Stage::Idle;
                }
                break;
            case Stage::Idle:
                level_ = 0.f;
                break;
            }

            out[i] = level_;
        }

        env_out_->Write(output);
    }

    // Data members
    float attack_ = 0.01f; // seconds
    float decay_ = 0.1f;   // seconds
    float sustain_ = 0.7f; // level 0..1
    float release_ = 0.2f; // seconds

    REFLECT_ENABLE(Module)

private:
    static constexpr float kGateThreshold = 0.5f;

    VoltageInput *gate_in_ = nullptr;
    VoltageOutput *env_out_ = nullptr;

    Stage stage_ = Stage::Idle;
    float level_ = 0.f;
    float release_rate_ = 0.f;
    bool gate_was_high_ = false;
};

} // namespace augr