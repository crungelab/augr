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
        auto detuneParam = CreateFloatParameter("Detune", ControlMeta::kSemitones, &detune_, 0.f, -24.f, 24.f, 0.01f);
        ui.Knob("Detune", detuneParam);

        std::vector<EnumParameterT<Waveform>::Choice> waveformChoices = {
            {Waveform::Saw, "Saw"},
            {Waveform::Square, "Square"},
            {Waveform::Tri, "Tri"},
            {Waveform::Sine, "Sine"}
        };
        auto waveformParam = CreateEnumParameter("Waveform", ControlMeta::kDefault, &waveform_, waveformChoices, Waveform::Saw);
        ui.Combo("Waveform", waveformParam);

        return true;
    }

    void Process() override {
        Audio pitch_cv = cv_pitch_in_->Read();
        Audio pw_cv = cv_pw_in_->Read();
        Audio output(ChannelLayout::kMono);

        float sr = Audio::sample_rate();
        int nFrames = Audio::frames();
        bool has_pitch = pitch_cv.layout_ != ChannelLayout::kNull;
        bool has_pw = pw_cv.layout_ != ChannelLayout::kNull;

        auto &out = output.array(); // shape [1][frames]

        for (int i = 0; i < nFrames; ++i) {
            float cv = has_pitch ? pitch_cv.array()(0, i) : 0.f;
            float pw =
                has_pw ? std::clamp(pw_cv.array()(0, i), 0.05f, 0.95f) : 0.5f;
            float freq = cvToFreq(cv + detune_ / 12.f);
            float dt = freq / sr;

            out(0, i) = tick(phase_, pw);
            phase_ = std::fmod(phase_ + dt, 1.f);
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
    float phase_ = 0.f;

    static float cvToFreq(float cv) { return 440.f * std::pow(2.f, cv); }

    float tick(float phase, float pw) {
        switch (waveform_) {
        case Waveform::Saw:
            return 2.f * phase - 1.f;
        case Waveform::Square:
            return phase < pw ? 1.f : -1.f;
        case Waveform::Tri:
            return 4.f * std::abs(phase - 0.5f) - 1.f;
        case Waveform::Sine:
            return std::sin(phase * 2.f * float(M_PI));
        }
        return 0.f;
    }
};

} // namespace augr