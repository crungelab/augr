// augr/fm/dexie.h
#pragma once

#include <augr/rack/module/module.h>
#include <augr/rack/voltage_pin.h>

#include <augr/fm/dexie_env.h>

namespace augr::fm {

// A self-contained DX7-style operator: phase-modulated sine with an
// integrated DX7 envelope generator (see DexieEnv). Carriers and modulators
// are structurally identical — distinguished only by wiring.
class Dexie : public Module {
public:
    void OnCreate() override;
    void CreateControls() override;
    void CreatePins() override;
    void Process() override;

    // --- Envelope generator (raw DX7 0..99 values) ---
    float rates_[4] = {99.f, 50.f, 50.f, 50.f};
    float levels_[4] = {99.f, 75.f, 50.f, 0.f};

    // --- Oscillator ---
    // Ratio to fundamental. Fixed-frequency mode active when ratio <= 0.
    float ratio_coarse_ = 1.0f;
    float ratio_fine_ = 0.0f;
    float frequency_ = 0.0f;     // Hz; used only when ratio <= 0
    float detune_ = 0.0f;        // DX7 detune: integer steps -7..7
    float output_level_ = 99.0f; // raw DX7 operator output level 0..99

    // Feedback: raw DX7 amount 0..7. Process() converts it to a feedback
    // shift internally (FEEDBACK_BITDEPTH - feedback). Only the algorithm's
    // designated feedback operator carries a nonzero value.
    float feedback_ = 0.0f;

    // CV inputs — pitch → gate → phase
    VoltageInput *cv_pitch_in_ = nullptr; // V/oct pitch
    VoltageInput *gate_in_ = nullptr;     // envelope gate (note on/off)
    VoltageInput *cv_phase_in_ = nullptr; // FM input — sum of modulator outputs
    VoltageInput *cv_amp_mod_in_ = nullptr; // shared voice LFO signal

    // Audio output
    AudioOutput *audio_out_ = nullptr;

    REFLECT_ENABLE(Module)
    // For UI display only: the peak absolute value of the current phase
    // modulation
    float phase_mod_peak_ = 0.0f;
    // temporary debug — last computed carrier/oscillator frequency, in Hz
    float debug_freq_ = 0.0f;

    // LFO stuff
    float amp_mod_sens_ = 0.0f; // 0..3, DX7 raw value (ampmodsenstab index)

private:
    // Oscillator state
    float phase_ = 0.0f;

    // Two-sample feedback history (stability fix — see compute_fb in MSFA /
    // Dexed). fb_hist_[0] is the most recent sample.
    float fb_hist_[2] = {0.0f, 0.0f};

    // Envelope
    DexieEnv env_;
    bool gate_prev_ = false;
};

} // namespace augr::fm