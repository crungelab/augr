// augr/fm/dx7_patch.h
#pragma once

#include <array>
#include <cstdint>
#include <span>
#include <string>
#include <vector>

namespace augr::fm {

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

struct Dx7Patch {
    std::string name;
    int algorithm = 0;        // 0..31
    int feedback = 0;         // 0..7
    std::array<Dx7Op, 6> ops; // ops[0] = OP1 .. ops[5] = OP6

    // LFO (voice-level, shared across all operators)
    int lfo_speed = 0;       // 0..99
    int lfo_delay = 0;       // 0..99
    int lfo_pitch_depth = 0; // 0..99
    int lfo_amp_depth = 0;   // 0..99
    int lfo_sync = 0;        // 0 or 1
    int lfo_waveform = 0;    // 0..5, DX7 encoding

    int pitch_mod_sens = 0; // 0..7, indexes pitchmodsenstab (not yet ported)

    int osc_key_sync = 0; // 0 or 1 — whether operators reset phase on note-on
    int transpose = 0;    // 0..48, 24=no transpose (raw DX7 value)
    int pitch_eg_rates[4] = {};  // pitch EG (deferred)
    int pitch_eg_levels[4] = {}; // pitch EG (deferred)
};

bool ParseDx7Voice(std::span<const uint8_t> data, Dx7Patch &out);
std::vector<Dx7Patch> ParseDx7Cartridge(std::span<const uint8_t> sysex);

} // namespace augr::fm