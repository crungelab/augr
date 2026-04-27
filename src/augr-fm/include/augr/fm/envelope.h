// augr/fm/envelope.h
#pragma once

#include <augr/rack/module/module.h>
#include <augr/rack/voltage_pin.h>

namespace augr::fm {

// DX-style 4-rate / 4-level envelope. Classic FM envelopes aren't ADSR —
// they're piecewise linear with a rate and target level per segment.
// Keeping it that way preserves the characteristic FM "snap" and makes
// the library feel period-correct.
class Envelope : public Module {
public:
    enum class Stage { kIdle, kR1, kR2, kR3, kR4 };

    bool Create(Part &owner) override;
    void CreateControls() override;
    void CreatePins() override;

    void Process() override;

    // Rates are 0..99 DX-style (log-curved internally).
    float rates_[4]  = { 99.f, 50.f, 50.f, 50.f };
    float levels_[4] = { 99.f, 75.f, 50.f,  0.f };

    VoltageInput  *gate_in_ = nullptr;
    VoltageOutput *cv_out_  = nullptr;

    REFLECT_ENABLE(Module)

private:
    Stage stage_ = Stage::kIdle;
    float value_ = 0.0f;
    bool  gate_prev_ = false;
};

} // namespace augr::fm