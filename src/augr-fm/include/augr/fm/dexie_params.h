#pragma once

namespace augr::fm {

/*
struct Dx7Op {
    int rates[4] = {99, 99, 99, 99};
    int levels[4] = {99, 99, 99, 0};
    int output_level = 99;
    int coarse = 1;
    int fine = 0;
    int detune = 0; // -7..7
    bool fixed_freq = false;
    int amp_mod_sens = 0;  // 0..3, raw DX7 AMS value
    int velocity_sens = 0; // 0..7, raw DX7 key velocity sensitivity

    // Keyboard level scaling — raw DX7 bytes, ported via ScaleLevel.
    int kbd_break_pt = 0;    // 0..99, offset by +17 internally
    int kbd_left_depth = 0;  // 0..99
    int kbd_right_depth = 0; // 0..99
    int kbd_left_curve = 0;  // 0..3
    int kbd_right_curve = 0; // 0..3
    int kbd_rate_scaling =
        0; // 0..7 — for pitch-dependent EG rate scaling (deferred)
};
*/
struct DexieParams {
    // All raw DX7 integer values — these are the canonical source of truth.
    // Serialized directly as integers. Computed values derived from these.
    int rates[4] = {99, 99, 99, 99};
    int levels[4] = {99, 99, 99, 0};
    int output_level = 99;
    int coarse = 1; // raw 0..31
    int fine = 0;   // raw 0..99
    int detune = 0; // centered -7..7
    bool fixed_freq = false;
    int feedback = 0;      // 0..7
    int amp_mod_sens = 0;  // 0..3
    int velocity_sens = 0; // 0..7
    int kbd_break_pt = 0;
    int kbd_left_depth = 0;
    int kbd_right_depth = 0;
    int kbd_left_curve = 0;
    int kbd_right_curve = 0;
    int kbd_rate_scaling = 0;
    // Global (per-patch) parameters that DexieVoice pushes to each operator:
    int lfo_pitch_depth = 0;
    int lfo_amp_depth = 99;
    int pitch_mod_sens = 0;
    bool osc_key_sync = false;
    int transpose = 0;
};

} // namespace augr::fm