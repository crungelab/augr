#pragma once

#include <augr/rack/module/module.h>
#include <augr/rack/voltage_pin.h>

namespace augr {

class ControlVoltage : public Module {
public:
    bool Create(Part &owner) override {
        Module::Create(owner);
        label_ = "CV";

        cv_out_ = new VoltageOutput(*this, "cv_out");
        AddOutput(*cv_out_);

        return true;
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