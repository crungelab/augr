#pragma once

#include <augr/core/ui/ui_builder.h>

#include <augr/rack/module/module.h>
#include <augr/rack/voltage_pin.h>

namespace augr {

/*
 * Control Voltage (CV) module with adjustable output voltage.
 */

class CvModule : public Module {
public:
    void Create(Model *parent = nullptr) override {
        Module::Create(parent);
        label_ = "CV";
    }

    void CreateControls() override {
        UiBuilder ui(*this);
        auto param = CreateFloatParameter("Voltage", ControlMeta::kDefault,
                                          &voltage_, 0.f, -4.f, 4.f, 0.01f);
        ui.Knob("Voltage", param);
    }

    void CreatePins() override {
        cv_out_ = new VoltageOutput(*this, "cv_out");
        AddOutput(*cv_out_);
    }

    void Process() override {
        Audio output(ChannelLayout::kMono);
        output.array().fill(voltage_);
        cv_out_->Write(output);
    }

    // Data members
    float voltage_ = 0.f;

    REFLECT_ENABLE(Module)

private:
    VoltageOutput *cv_out_ = nullptr;
};

} // namespace augr