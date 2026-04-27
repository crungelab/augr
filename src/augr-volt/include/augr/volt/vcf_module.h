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

/*
* Voltage Controlled Filter (VCF) module. State-variable filter with cutoff and resonance control, plus CV inputs for both. Based on the TPT SVF topology:
* https://www.earlevel.com/main/2012/11/26/biquads-vstpt-svfs-and-their-variations/
*/

class VcfModule : public Module {
public:
    bool Create(Part &owner) override {
        Module::Create(owner);
        label_ = "VCF";

        audio_in_ = new AudioInput(*this, "audio_in", ChannelLayout::kMono);
        AddInput(*audio_in_);

        cv_cutoff_in_ = new VoltageInput(*this, "cv_cutoff_in");
        AddInput(*cv_cutoff_in_);

        cv_resonance_in_ = new VoltageInput(*this, "cv_resonance_in");
        AddInput(*cv_resonance_in_);

        audio_out_ = new AudioOutput(*this, "audio_out", ChannelLayout::kMono);
        AddOutput(*audio_out_);
        /*
        UiBuilder ui(*this);

        // Cutoff stored in octaves relative to C4 (same convention as VCO
        // pitch). 0 = 261.6 Hz (C4), +3 = 2093 Hz (C7), -3 = 32.7 Hz (C1).
        auto cutoffParam = CreateFloatParameter(
            "Cutoff", ControlMeta::kDefault, &cutoff_, 2.f, -4.f, 6.f, 0.01f);
        ui.Knob("Cutoff", cutoffParam);

        auto resonanceParam =
            CreateFloatParameter("Resonance", ControlMeta::kDefault,
                                 &resonance_, 0.f, 0.f, 1.f, 0.01f);
        ui.Knob("Resonance", resonanceParam);
        */
        return true;
    }

    void CreateControls() override {
        UiBuilder ui(*this);

        auto cutoffParam = CreateFloatParameter(
            "Cutoff", ControlMeta::kDefault, &cutoff_, 2.f, -4.f, 6.f, 0.01f);
        ui.Knob("Cutoff", cutoffParam);

        auto resonanceParam =
            CreateFloatParameter("Resonance", ControlMeta::kDefault,
                                 &resonance_, 0.f, 0.f, 1.f, 0.01f);
        ui.Knob("Resonance", resonanceParam);
    }

    void Process() override {
        Audio audio_in = audio_in_->Read();
        Audio cutoff_cv = cv_cutoff_in_->Read();
        Audio resonance_cv = cv_resonance_in_->Read();
        Audio output(ChannelLayout::kMono);

        const int nFrames = Audio::frames();
        const float sr = Audio::sample_rate();
        const bool has_audio = audio_in.layout_ != ChannelLayout::kNull;
        const bool has_cutoff_cv = cutoff_cv.layout_ != ChannelLayout::kNull;
        const bool has_res_cv = resonance_cv.layout_ != ChannelLayout::kNull;

        fy_real *out = output.array().data();

        if (!has_audio) {
            for (int i = 0; i < nFrames; ++i)
                out[i] = 0.f;
            audio_out_->Write(output);
            return;
        }

        const fy_real *in = audio_in.array().data();
        const fy_real *cutoff_buf =
            has_cutoff_cv ? cutoff_cv.array().data() : nullptr;
        const fy_real *res_buf =
            has_res_cv ? resonance_cv.array().data() : nullptr;

        // Nyquist limit on cutoff (with a small safety margin).
        const float max_freq = sr * 0.49f;

        for (int i = 0; i < nFrames; ++i) {
            // Cutoff in octaves relative to C4, plus CV (1V/oct).
            const float cutoff_oct =
                cutoff_ + (has_cutoff_cv ? float(cutoff_buf[i]) : 0.f);
            float freq = 261.6255653f * std::pow(2.f, cutoff_oct);
            freq = std::clamp(freq, 20.f, max_freq);

            // Resonance knob + CV, clamped to stable range.
            const float res_cv_val = has_res_cv ? float(res_buf[i]) : 0.f;
            const float q_norm =
                std::clamp(resonance_ + res_cv_val, 0.f, 0.99f);

            // Prewarped cutoff coefficient.
            const float g = std::tan(float(M_PI) * freq / sr);
            // Damping: k = 1/Q. Lower k = higher resonance.
            // Map q_norm [0..0.99] to k [2..0.02].
            const float k = 2.f * (1.f - q_norm);
            const float a1 = 1.f / (1.f + g * (g + k));
            const float a2 = g * a1;
            const float a3 = g * a2;

            const float v0 = float(in[i]);
            const float v3 = v0 - ic2eq_;
            const float v1 = a1 * ic1eq_ + a2 * v3;
            const float v2 = ic2eq_ + a2 * ic1eq_ + a3 * v3;
            ic1eq_ = 2.f * v1 - ic1eq_;
            ic2eq_ = 2.f * v2 - ic2eq_;

            // v2 is the lowpass output. (v0 - k*v1 - v2) would be highpass, v1
            // is bandpass.
            out[i] = v2;
        }

        audio_out_->Write(output);
    }

    // Data members
    float cutoff_ = 2.f;    // octaves above C4; default +2 = ~1 kHz
    float resonance_ = 0.f; // 0..1

    REFLECT_ENABLE(Module)

private:
    AudioInput *audio_in_ = nullptr;
    VoltageInput *cv_cutoff_in_ = nullptr;
    VoltageInput *cv_resonance_in_ = nullptr;
    AudioOutput *audio_out_ = nullptr;

    // TPT SVF integrator states
    float ic1eq_ = 0.f;
    float ic2eq_ = 0.f;
};

} // namespace augr