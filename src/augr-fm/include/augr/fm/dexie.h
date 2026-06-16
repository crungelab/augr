// augr/fm/dexie.h
#pragma once

#include <augr/rack/module/module.h>
#include <augr/rack/voltage_pin.h>

namespace augr::fm {

// A self-contained DX7-style operator: phase-modulated sine with an
// integrated 4-rate/4-level envelope generator. Carriers and modulators
// are structurally identical — distinguished only by wiring.
class Dexie : public Module {
public:
    enum class Stage { kIdle, kR1, kR2, kR3, kR4 };

    void OnCreate() override;
    void CreateControls() override;
    void CreatePins() override;
    void Process() override;

    // --- Envelope generator (DX7 EG) ---
    // Rates and levels are 0..99 DX-style, log-curved internally.
    float rates_[4] = {99.f, 50.f, 50.f, 50.f};
    float levels_[4] = {99.f, 75.f, 50.f, 0.f};

    // --- Oscillator ---
    // Ratio to fundamental. Fixed-frequency mode active when ratio <= 0.
    float ratio_coarse_ = 1.0f;
    float ratio_fine_ = 0.0f;
    float frequency_ = 0.0f; // Hz; used only when ratio <= 0
    float detune_ = 0.0f; // DX7 detune: integer steps -7..7, ~0.49 cents each
    float output_level_ = 1.0f; // operator output scaling [0..1]
    // Feedback: feedback_shift_ follows the DX7's own encoding — derived from
    // the raw 0..7 patch value as FEEDBACK_BITDEPTH - feedback (or 16 if the
    // patch feedback is 0, meaning "off"). See Dx7Note::init in Dexed's Mark I
    // engine. Only the algorithm's designated feedback operator has a value
    // other than 16; all others stay off.
    int feedback_shift_ = 16;
    float feedback_ =
        0.0f; // raw DX7 feedback amount, 0..7 — same range as the
              // hardware control. Converted to a shift internally.

    // CV inputs — pitch → gate → phase
    VoltageInput *cv_pitch_in_ = nullptr; // V/oct pitch
    VoltageInput *gate_in_ = nullptr;     // envelope gate (note on/off)
    VoltageInput *cv_phase_in_ = nullptr; // FM input — sum of modulator outputs

    // Audio output
    AudioOutput *audio_out_ = nullptr;

    REFLECT_ENABLE(Module)

private:
    // Oscillator state
    float phase_ = 0.0f;
    // float last_sample_ = 0.0f;  // for feedback
    // In Dexie, replace last_sample_ with a 2-sample history:
    float fb_hist_[2] = {0.f, 0.f};

    // Envelope state
    Stage stage_ = Stage::kIdle;
    float eg_value_ = 0.0f;
    bool gate_prev_ = false;
};

} // namespace augr::fm