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
        return true;
    }

    void CreatePins() override {
        cv_pitch_in_ = new VoltageInput(*this, "cv_pitch_in");
        AddInput(*cv_pitch_in_);

        cv_pw_in_ = new VoltageInput(*this, "cv_pw_in");
        AddInput(*cv_pw_in_);

        audio_fm_in_ = new AudioInput(*this, "audio_fm_in", ChannelLayout::kMono);
        AddInput(*audio_fm_in_);

        audio_out_ = new AudioOutput(*this, "audio_out", ChannelLayout::kMono);
        AddOutput(*audio_out_);
    }

    void CreateControls() override {
        UiBuilder ui(*this);

        auto detuneParam = CreateFloatParameter("Detune", ControlMeta::kSemitones, &detune_, 0.f, -24.f, 24.f, 0.01f);
        ui.Knob("Detune", detuneParam);

        auto fmDepthParam = CreateFloatParameter("FM Depth", ControlMeta::kDefault, &fm_depth_, 0.f, 0.f, 4.f, 0.01f);
        ui.Knob("FM Depth", fmDepthParam);

        std::vector<EnumParameterT<Waveform>::Choice> waveformChoices = {
            {Waveform::Saw, "Saw"},
            {Waveform::Square, "Square"},
            {Waveform::Tri, "Tri"},
            {Waveform::Sine, "Sine"}
        };
        auto waveformParam = CreateEnumParameter("Waveform", ControlMeta::kDefault, &waveform_, waveformChoices, Waveform::Saw);
        ui.Combo("Waveform", waveformParam);
    }

    void Process() override {
        Audio pitch_cv = cv_pitch_in_->Read();
        Audio pw_cv = cv_pw_in_->Read();
        Audio fm_audio = audio_fm_in_->Read();
        Audio output(ChannelLayout::kMono);

        const float sr = Audio::sample_rate();
        const int nFrames = Audio::frames();
        const bool has_pitch = pitch_cv.layout_ != ChannelLayout::kNull;
        const bool has_pw = pw_cv.layout_ != ChannelLayout::kNull;
        const bool has_fm = fm_audio.layout_ != ChannelLayout::kNull && fm_depth_ > 0.f;
        const float detune_octaves = detune_ / 12.f;

        fy_real *out = output.array().data();
        const fy_real *pitch = has_pitch ? pitch_cv.array().data() : nullptr;
        const fy_real *pw_buf = has_pw ? pw_cv.array().data() : nullptr;
        const fy_real *fm_buf = has_fm ? fm_audio.array().data() : nullptr;

        for (int i = 0; i < nFrames; ++i) {
            const float cv = has_pitch ? float(pitch[i]) : 0.f;
            const float pw = has_pw ? std::clamp(float(pw_buf[i]), 0.05f, 0.95f) : 0.5f;
            const float freq = cvToFreq(cv + detune_octaves);
            const float dt = freq / sr;

            // Phase offset from FM input. fm_depth_ in "cycles" — depth of 1
            // means a modulator at ±1 shifts phase by ±1 full cycle.
            const float phase_offset = has_fm ? float(fm_buf[i]) * fm_depth_ : 0.f;
            const float modulated_phase = wrap01(phase_ + phase_offset);

            out[i] = tick(modulated_phase, dt, pw);

            phase_ += dt;
            if (phase_ >= 1.f) phase_ -= 1.f;
        }

        audio_out_->Write(output);
    }

    // Data members
    Waveform waveform_ = Waveform::Saw;
    float detune_ = 0.f;      // semitones
    float fm_depth_ = 0.f;    // cycles of phase offset per unit modulator input

    REFLECT_ENABLE(Module)

private:
    VoltageInput *cv_pitch_in_ = nullptr;
    VoltageInput *cv_pw_in_ = nullptr;
    AudioInput *audio_fm_in_ = nullptr;
    AudioOutput *audio_out_ = nullptr;
    float phase_ = 0.f;

    // 0V = C4 (middle C, ~261.63 Hz), 1V/octave
    static float cvToFreq(float cv) { return 261.6255653f * std::pow(2.f, cv); }

    // Wrap phase into [0, 1), handling negative offsets and multiple wraps.
    static float wrap01(float phase) {
        phase -= std::floor(phase);
        return phase;
    }

    // PolyBLEP: polynomial band-limited step correction.
    static float polyblep(float t, float dt) {
        if (t < dt) {
            t /= dt;
            return t + t - t * t - 1.f;
        } else if (t > 1.f - dt) {
            t = (t - 1.f) / dt;
            return t * t + t + t + 1.f;
        }
        return 0.f;
    }

    float tick(float phase, float dt, float pw) {
        switch (waveform_) {
        case Waveform::Saw: {
            float v = 2.f * phase - 1.f;
            v -= polyblep(phase, dt);
            return v;
        }
        case Waveform::Square: {
            float v = phase < pw ? 1.f : -1.f;
            v += polyblep(phase, dt);
            v -= polyblep(wrap01(phase - pw + 1.f), dt);
            return v;
        }
        case Waveform::Tri:
            return 4.f * std::abs(phase - 0.5f) - 1.f;
        case Waveform::Sine:
            return std::sin(phase * 2.f * float(M_PI));
        }
        return 0.f;
    }
};

} // namespace augr