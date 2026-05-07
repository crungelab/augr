// augr/fm/operator.h
#pragma once

#include <augr/rack/module/module.h>
#include <augr/rack/voltage_pin.h>

namespace augr::fm {

// A single FM operator: phase-modulated sine + envelope.
// The operator is the atomic unit of FM synthesis — carriers and modulators
// are structurally identical, distinguished only by how they're wired.
class Operator : public Module {
public:
    void Create(Part *owner = nullptr) override;
    void CreateControls() override;
    void CreatePins() override;
    void Process() override;

    // Ratio to fundamental (e.g. 1.0 = carrier pitch, 2.0 = octave up).
    // Fixed-frequency mode uses frequency_ directly when ratio_ <= 0.
    float detune_ = 0.f;  // -7 to 7, stored as float, treated as integer
    float ratio_coarse_ = 1.0f;
    float ratio_fine_   = 0.0f;
    float frequency_ = 0.0f;  // Hz, used when ratio <= 0
    float output_level_ = 1.0f;
    float feedback_ = 0.0f;   // self-modulation amount [0..1]

    // CV inputs
    VoltageInput *cv_pitch_in_ = nullptr;   // V/oct or Hz, consistent with VCO
    VoltageInput *cv_level_in_ = nullptr;   // envelope / amp mod
    VoltageInput *cv_phase_in_ = nullptr;   // the FM input — sum of modulators

    REFLECT_ENABLE(Module)

private:
    float phase_ = 0.0f;
    float last_sample_ = 0.0f;  // for feedback
};

} // namespace augr::fm