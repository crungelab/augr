#pragma once

#include <augr/core/audio.h>
#include <augr/core/ui/ui_builder.h>

#include <augr/rack/audio_pin.h>
#include <augr/rack/module/module.h>
#include <augr/rack/node.h>
#include <augr/rack/voltage_pin.h>

namespace augr {

/*
* Voltage Controlled Amplifier (VCA) module with audio input, CV gain control, and audio output. Gain parameter allows adjusting the overall amplification.
*/

class VcaModule : public Module {
public:
    bool Create(Part &owner) override {
        Module::Create(owner);
        label_ = "VCA";

        audio_in_ = new AudioInput(*this, "audio_in", ChannelLayout::kMono);
        AddInput(*audio_in_);

        cv_gain_in_ = new VoltageInput(*this, "cv_gain_in");
        AddInput(*cv_gain_in_);

        audio_out_ = new AudioOutput(*this, "audio_out", ChannelLayout::kMono);
        AddOutput(*audio_out_);

        UiBuilder ui(*this);
        auto gainParam = CreateFloatParameter("Gain", ControlMeta::kDefault,
                                              &gain_, 1.f, 0.f, 1.f, 0.01f);
        ui.Knob("Gain", gainParam);

        return true;
    }

    void Process() override {
        Audio audio_in = audio_in_->Read();
        Audio cv_gain = cv_gain_in_->Read();
        Audio output(ChannelLayout::kMono);

        const int nFrames = Audio::frames();
        const bool has_audio = audio_in.layout_ != ChannelLayout::kNull;
        const bool has_cv = cv_gain.layout_ != ChannelLayout::kNull;

        fy_real *out = output.array().data();

        if (!has_audio) {
            for (int i = 0; i < nFrames; ++i) {
                out[i] = 0.f;
            }
            audio_out_->Write(output);
            return;
        }

        const fy_real *in = audio_in.array().data();

        if (has_cv) {
            const fy_real *cv = cv_gain.array().data();
            for (int i = 0; i < nFrames; ++i) {
                const float g = std::clamp(float(cv[i]), 0.f, 1.f) * gain_;
                out[i] = in[i] * g;
            }
        } else {
            for (int i = 0; i < nFrames; ++i) {
                out[i] = in[i] * gain_;
            }
        }

        audio_out_->Write(output);
    }

    // Data members
    float gain_ = 1.f;

    REFLECT_ENABLE(Module)

private:
    AudioInput *audio_in_ = nullptr;
    VoltageInput *cv_gain_in_ = nullptr;
    AudioOutput *audio_out_ = nullptr;
};

} // namespace augr