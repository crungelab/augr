#pragma once

#include <augr/core/audio.h>
#include <augr/core/ui/ui_builder.h>

#include <augr/rack/module/module.h>
#include <augr/rack/node.h>
#include <augr/rack/voltage_pin.h>

namespace augr {

class AttenuverterModule : public Module {
public:
    bool Create(Part &owner) override {
        Module::Create(owner);
        label_ = "Atten";
        return true;
    }

    void CreatePins() override {
        cv_in_ = new VoltageInput(*this, "cv_in");
        AddInput(*cv_in_);

        cv_out_ = new VoltageOutput(*this, "cv_out");
        AddOutput(*cv_out_);
    }

    void CreateControls() override {
        UiBuilder ui(*this);

        // Bipolar amount: negative inverts, zero silences, positive scales up.
        auto amountParam = CreateFloatParameter(
            "Amount", ControlMeta::kDefault, &amount_, 1.f, -5.f, 5.f, 0.01f);
        ui.Knob("Amount", amountParam);

        // DC offset added after scaling. Useful for biasing a unipolar CV into
        // bipolar range (or vice versa), or for manual CV when nothing is
        // patched.
        auto offsetParam = CreateFloatParameter(
            "Offset", ControlMeta::kDefault, &offset_, 0.f, -5.f, 5.f, 0.01f);
        ui.Knob("Offset", offsetParam);
    }

    void Process() override {
        Audio cv_in = cv_in_->Read();
        Audio output(ChannelLayout::kMono);

        const int nFrames = Audio::frames();
        const bool has_cv = cv_in.layout_ != ChannelLayout::kNull;

        fy_real *out = output.array().data();

        if (has_cv) {
            const fy_real *in = cv_in.array().data();
            for (int i = 0; i < nFrames; ++i) {
                out[i] = float(in[i]) * amount_ + offset_;
            }
        } else {
            // No input patched — output is just the offset (module acts as a
            // manual CV source).
            for (int i = 0; i < nFrames; ++i) {
                out[i] = offset_;
            }
        }

        cv_out_->Write(output);
    }

    // Data members
    float amount_ = 1.f; // bipolar scale factor
    float offset_ = 0.f; // added DC offset in volts

    REFLECT_ENABLE(Module)

private:
    VoltageInput *cv_in_ = nullptr;
    VoltageOutput *cv_out_ = nullptr;
};

} // namespace augr