#pragma once

#include <array>

#include <augr/core/audio.h>
#include <augr/core/ui/ui_builder.h>

#include <augr/rack/audio_pin.h>
#include <augr/rack/module/module.h>
#include <augr/rack/node.h>

namespace augr {

class MixerModule : public Module {
public:
    static constexpr int kNumChannels = 4;

    void Create() override {
        Module::Create();
        label_ = "Mixer";
    }

    void CreateControls() override {
        UiBuilder ui(*this);

        for (int ch = 0; ch < kNumChannels; ++ch) {
            const std::string label = "Ch " + std::to_string(ch + 1);
            auto param = CreateFloatParameter(label, ControlMeta::kDefault, &levels_[ch], 1.f, -1.f, 1.f, 0.01f);
            ui.Knob(label, param);
        }

        auto masterParam = CreateFloatParameter("Master", ControlMeta::kDefault, &master_, 1.f, 0.f, 1.f, 0.01f);
        ui.Knob("Master", masterParam);        
    }

    void CreatePins() override {
        for (int ch = 0; ch < kNumChannels; ++ch) {
            const std::string name = "audio_in_" + std::to_string(ch + 1);
            audio_in_[ch] = new AudioInput(*this, name, ChannelLayout::kMono);
            AddInput(*audio_in_[ch]);
        }

        audio_out_ = new AudioOutput(*this, "audio_out", ChannelLayout::kMono);
        AddOutput(*audio_out_);
    }

    void Process() override {
        Audio output(ChannelLayout::kMono);
        const int nFrames = Audio::frames();

        fy_real *out = output.array().data();

        // Read all inputs up front. Skip disconnected channels.
        std::array<Audio, kNumChannels> inputs;
        std::array<const fy_real *, kNumChannels> bufs{};
        std::array<bool, kNumChannels> active{};
        for (int ch = 0; ch < kNumChannels; ++ch) {
            inputs[ch] = audio_in_[ch]->Read();
            active[ch] = inputs[ch].layout() != ChannelLayout::kNull && levels_[ch] != 0.f;
            bufs[ch] = active[ch] ? inputs[ch].array().data() : nullptr;
        }

        for (int i = 0; i < nFrames; ++i) {
            float sum = 0.f;
            for (int ch = 0; ch < kNumChannels; ++ch) {
                if (active[ch]) {
                    sum += float(bufs[ch][i]) * levels_[ch];
                }
            }
            out[i] = sum * master_;
        }

        audio_out_->Write(output);
    }

    // Data members
    std::array<float, kNumChannels> levels_{1.f, 1.f, 1.f, 1.f};
    float master_ = 1.f;

    REFLECT_ENABLE(Module)

private:
    std::array<AudioInput *, kNumChannels> audio_in_{};
    AudioOutput *audio_out_ = nullptr;
};

} // namespace augr