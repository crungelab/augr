#pragma once

#include <cstdint>
#include <vector>

#include <augr/audio.h>
#include <augr/ui/ui_builder.h>

#include <augr/rack/audio_pin.h>
#include <augr/rack/module/module.h>
#include <augr/rack/node.h>

namespace augr {

class NoiseModule : public Module {
public:
    enum class Color { White, Pink, Brown };

    void OnCreate() override {
        Module::OnCreate();
        label_ = "Noise";
    }

    void CreateControls() override {
        UiBuilder ui(shared_from_this());

        auto levelParam = CreateFloatParameter("Level", ControlMeta::kDefault,
                                               &level_, 1.f, 0.f, 1.f, 0.01f);
        ui.Knob("Level", levelParam);

        std::vector<EnumParameterT<Color>::Choice> colorChoices = {
            {Color::White, "White"},
            {Color::Pink, "Pink"},
            {Color::Brown, "Brown"},
        };
        auto colorParam = CreateEnumParameter(
            "Color", ControlMeta::kDefault, &color_, colorChoices, Color::Pink);
        ui.Combo("Color", colorParam);
    }

    void CreatePins() override {
        audio_out_ = new AudioOutput(*this, "audio_out", ChannelLayout::kMono);
        AddOutput(*audio_out_);
    }

    void Process() override {
        Audio output(ChannelLayout::kMono);
        const int nFrames = Audio::frames();

        fy_real *out = output.array().data();

        switch (color_) {
        case Color::White:
            for (int i = 0; i < nFrames; ++i) {
                out[i] = NextWhite() * level_;
            }
            break;
        case Color::Pink:
            for (int i = 0; i < nFrames; ++i) {
                out[i] = NextPink() * level_;
            }
            break;
        case Color::Brown:
            for (int i = 0; i < nFrames; ++i) {
                out[i] = NextBrown() * level_;
            }
            break;
        }

        audio_out_->Write(output);
    }

    // Data members
    Color color_ = Color::Pink;
    float level_ = 1.f;

    REFLECT_ENABLE(Module)

private:
    AudioOutput *audio_out_ = nullptr;

    // xorshift32 PRNG state.
    uint32_t rng_state_ = 0x9E3779B9u;

    // Pink noise state (Paul Kellet's economy filter).
    float pink_b0_ = 0.f;
    float pink_b1_ = 0.f;
    float pink_b2_ = 0.f;
    float pink_b3_ = 0.f;
    float pink_b4_ = 0.f;
    float pink_b5_ = 0.f;
    float pink_b6_ = 0.f;

    // Brown noise state (leaky integrator of white noise).
    float brown_last_ = 0.f;

    // Returns a uniform random float in [-1, 1].
    float NextWhite() {
        rng_state_ ^= rng_state_ << 13;
        rng_state_ ^= rng_state_ >> 17;
        rng_state_ ^= rng_state_ << 5;
        return (float(rng_state_) / float(UINT32_MAX)) * 2.f - 1.f;
    }

    // Paul Kellet's pink noise filter. Approximates a -3 dB/octave slope from
    // ~10 Hz to ~20 kHz. Output roughly in [-1, 1].
    float NextPink() {
        const float w = NextWhite();
        pink_b0_ = 0.99886f * pink_b0_ + w * 0.0555179f;
        pink_b1_ = 0.99332f * pink_b1_ + w * 0.0750759f;
        pink_b2_ = 0.96900f * pink_b2_ + w * 0.1538520f;
        pink_b3_ = 0.86650f * pink_b3_ + w * 0.3104856f;
        pink_b4_ = 0.55000f * pink_b4_ + w * 0.5329522f;
        pink_b5_ = -0.7616f * pink_b5_ - w * 0.0168980f;
        const float pink = pink_b0_ + pink_b1_ + pink_b2_ + pink_b3_ +
                           pink_b4_ + pink_b5_ + pink_b6_ + w * 0.5362f;
        pink_b6_ = w * 0.115926f;
        // Kellet's sum averages ~5-6x larger than white. Scale to [-1, 1]-ish.
        return pink * 0.11f;
    }

    // Brown noise: leaky integrator of white noise.
    // Leak factor prevents DC drift; gain compensates for integration.
    float NextBrown() {
        const float w = NextWhite();
        brown_last_ = 0.996f * brown_last_ + w * 0.05f;
        // Output roughly in [-1, 1] at steady state.
        return brown_last_ * 3.5f;
    }
};

} // namespace augr