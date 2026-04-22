#pragma once

#include <algorithm>
#include <cmath>

#include <augr/core/audio.h>
#include <augr/core/ui/ui_builder.h>

#include <augr/rack/audio_pin.h>
#include <augr/rack/module/module.h>
#include <augr/rack/node.h>
#include <augr/rack/voltage_pin.h>

namespace augr {

class VcoModule : public Module {
public:
    enum class Waveform { Saw, Square, Tri, Sine };

    bool Create(Part &owner) override {
        Module::Create(owner);
        label_ = "VCO";

        cv_pitch_in_ = new VoltageInput(*this, "cv_pitch_in");
        AddInput(*cv_pitch_in_);

        cv_pw_in_ = new VoltageInput(*this, "cv_pw_in");
        AddInput(*cv_pw_in_);

        audio_out_ = new AudioOutput(*this, "audio_out", ChannelLayout::kMono);
        AddOutput(*audio_out_);

        UiBuilder ui(*this);
        auto detuneParam =
            CreateFloatParameter("Detune", ControlMeta::kSemitones, &detune_,
                                 0.f, -24.f, 24.f, 0.01f);
        ui.Knob("Detune", detuneParam);

        std::vector<EnumParameterT<Waveform>::Choice> waveformChoices = {
            {Waveform::Saw, "Saw"},
            {Waveform::Square, "Square"},
            {Waveform::Tri, "Tri"},
            {Waveform::Sine, "Sine"}};
        auto waveformParam =
            CreateEnumParameter("Waveform", ControlMeta::kDefault, &waveform_,
                                waveformChoices, Waveform::Saw);
        ui.Combo("Waveform", waveformParam);

        return true;
    }

    void Process() override {
        Audio pitch_cv = cv_pitch_in_->Read();
        Audio pw_cv = cv_pw_in_->Read();
        Audio output(ChannelLayout::kMono);

        const float sr = Audio::sample_rate();
        const int nFrames = Audio::frames();
        const bool has_pitch = pitch_cv.layout_ != ChannelLayout::kNull;
        const bool has_pw = pw_cv.layout_ != ChannelLayout::kNull;
        const float detune_octaves = detune_ / 12.f;

        fy_real *out = output.array().data();
        const fy_real *pitch = has_pitch ? pitch_cv.array().data() : nullptr;
        const fy_real *pw_buf = has_pw ? pw_cv.array().data() : nullptr;

        if (has_pitch && has_pw) {
            for (int i = 0; i < nFrames; ++i) {
                const float pw = std::clamp(float(pw_buf[i]), 0.05f, 0.95f);
                const float freq = cvToFreq(float(pitch[i]) + detune_octaves);
                const float dt = freq / sr;
                out[i] = tick(phase_, dt, pw);
                phase_ = advance(phase_, dt);
            }
        } else if (has_pitch) {
            for (int i = 0; i < nFrames; ++i) {
                const float freq = cvToFreq(float(pitch[i]) + detune_octaves);
                const float dt = freq / sr;
                out[i] = tick(phase_, dt, 0.5f);
                phase_ = advance(phase_, dt);
            }
        } else if (has_pw) {
            const float freq = cvToFreq(detune_octaves);
            const float dt = freq / sr;
            for (int i = 0; i < nFrames; ++i) {
                const float pw = std::clamp(float(pw_buf[i]), 0.05f, 0.95f);
                out[i] = tick(phase_, dt, pw);
                phase_ = advance(phase_, dt);
            }
        } else {
            const float freq = cvToFreq(detune_octaves);
            const float dt = freq / sr;
            for (int i = 0; i < nFrames; ++i) {
                out[i] = tick(phase_, dt, 0.5f);
                phase_ = advance(phase_, dt);
            }
        }

        audio_out_->Write(output);
    }

    // Data members
    Waveform waveform_ = Waveform::Saw;
    float detune_ = 0.f; // semitones

    REFLECT_ENABLE(Module)

private:
    VoltageInput *cv_pitch_in_ = nullptr;
    VoltageInput *cv_pw_in_ = nullptr;
    AudioOutput *audio_out_ = nullptr;
    float phase_ = 0.f;

    // 0V = C4 (middle C, ~261.63 Hz), 1V/octave
    static float cvToFreq(float cv) { return 261.6255653f * std::pow(2.f, cv); }

    static float advance(float phase, float dt) {
        phase += dt;
        if (phase >= 1.f)
            phase -= 1.f;
        return phase;
    }

    // PolyBLEP: polynomial band-limited step correction.
    // Returns a value to add (or subtract) near a discontinuity to suppress
    // aliasing. |t| is the distance from the discontinuity, normalized by dt.
    static float polyblep(float t, float dt) {
        if (t < dt) {
            // Just after the discontinuity.
            t /= dt;
            return t + t - t * t - 1.f;
        } else if (t > 1.f - dt) {
            // Just before the next discontinuity.
            t = (t - 1.f) / dt;
            return t * t + t + t + 1.f;
        }
        return 0.f;
    }

    float tick(float phase, float dt, float pw) {
        switch (waveform_) {
        case Waveform::Saw: {
            // Naive saw with a downward discontinuity at phase 0 / 1.
            float v = 2.f * phase - 1.f;
            v -= polyblep(phase, dt);
            return v;
        }
        case Waveform::Square: {
            // Naive square: +1 when phase < pw, -1 otherwise.
            // Upward discontinuity at phase 0, downward at phase pw.
            float v = phase < pw ? 1.f : -1.f;
            v += polyblep(phase, dt); // rising edge at 0
            v -= polyblep(std::fmod(phase - pw + 1.f, 1.f),
                          dt); // falling edge at pw
            return v;
        }
        case Waveform::Tri: {
            // Triangle: integrate a PolyBLEP-corrected square.
            // Slope discontinuities alias less audibly; basic triangle is fine
            // for now.
            return 4.f * std::abs(phase - 0.5f) - 1.f;
        }
        case Waveform::Sine:
            return std::sin(phase * 2.f * float(M_PI));
        }
        return 0.f;
    }
};

} // namespace augr