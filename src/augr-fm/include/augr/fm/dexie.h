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
    bool  fixed_freq_ = false;  // true when operator ignores MIDI pitch

    float detune_ = 0.0f;        // DX7 detune: integer steps -7..7
    float output_level_ = 99.0f; // raw DX7 operator output level 0..99

    // Feedback: raw DX7 amount 0..7. Process() converts it to a feedback
    // shift internally (FEEDBACK_BITDEPTH - feedback). Only the algorithm's
    // designated feedback operator carries a nonzero value.
    float feedback_ = 0.0f;

    // --- LFO amplitude modulation ---
    float amp_mod_sens_ = 0.0f; // 0..3, raw DX7 AMS value (per-operator)
    /*
    float lfo_amp_depth_ =
        1.0f; // 0..1, from the patch's voice-level LFO amp depth
    */
    float lfo_amp_depth_ = 99.0f; // raw DX7 0..99 LFO amplitude-mod depth (AMD)

    int lfo_delay_samples_total_ =
        0; // samples for the LFO to ramp to full depth after gate-on

    float velocity_sens_ = 0.0f; // raw DX7 0..7 key velocity sensitivity

    // Keyboard
    int kbd_break_pt_ = 0;
    int kbd_left_depth_ = 0;
    int kbd_right_depth_ = 0;
    int kbd_left_curve_ = 0;
    int kbd_right_curve_ = 0;

    float kbd_rate_scaling_ = 0.0f; // 0..7, DX7 keyboard rate scaling

    // Pitch modulation (vibrato) — voice-level LFO path
    float lfo_pitch_depth_ = 0.0f; // raw DX7 0..99, voice-level
    float pitch_mod_sens_ = 0.0f;  // raw DX7 0..7, indexes pitchmodsenstab

    bool osc_key_sync_ = false;  // if true, reset phase on every note-on

    // CV inputs — pitch → gate → phase → amp mod
    VoltageInput *cv_pitch_in_ = nullptr; // V/oct pitch
    VoltageInput *gate_in_ = nullptr;     // envelope gate (note on/off)
    //VoltageInput *cv_phase_in_ = nullptr; // FM input — sum of modulator outputs
    MixingAudioInput *cv_phase_in_ = nullptr; // FM input — sum of modulator outputs
    VoltageInput *cv_amp_mod_in_ = nullptr; // shared voice LFO signal
    VoltageInput *cv_velocity_in_ =
        nullptr; // note-on velocity, 0..1 normalized
    VoltageInput *cv_pitch_mod_in_ =
        nullptr;                   // raw LFO signal, same source as amp mod

    // Audio output
    AudioOutput *audio_out_ = nullptr;

    // For UI display only: the peak absolute value of the current phase
    // modulation
    float phase_mod_peak_ = 0.0f;
    // temporary debug — last computed carrier/oscillator frequency, in Hz
    float debug_freq_ = 0.0f;

    REFLECT_ENABLE(Module)

private:
    // Oscillator state
    float phase_ = 0.0f;

    // Two-sample feedback history (stability fix — see compute_fb in MSFA /
    // Dexed). fb_hist_[0] is the most recent sample.
    float fb_hist_[2] = {0.0f, 0.0f};

    // Envelope
    DexieEnv env_;
    bool gate_prev_ = false;

    // LFO delay-ramp state — samples elapsed since the gate last went high.
    int samples_since_gate_on_ = 0;
};

} // namespace augr::fm