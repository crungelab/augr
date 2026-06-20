#pragma once

#include <cmath>

namespace augr::fm {

// Fixed-frequency mode: operator ignores MIDI note, runs at a fixed Hz.
// coarse_raw is the raw 0..31 patch byte; only bits 0-1 are used (& 3).
// fine_raw is raw 0..99. Reference 3.2 Hz derived from DX7 hardware
// measurements. Ported from Dexed's osc_freq fixed-frequency branch

inline float FixedFrequencyHz(int coarse_raw, int fine_raw, int detune_raw) {
    const int coarse   = coarse_raw & 3;
    const int combined = coarse * 100 + fine_raw;
    const float logfreq = (4458616.0f * combined) / 8.0f;
    constexpr float kFixedFreqRef = 3.2f;
    float hz = kFixedFreqRef * std::pow(2.0f, logfreq / 16777216.0f);

    // Fixed-frequency detune: only positive (raw detune > 7 = center).
    // Ported from osc_freq fixed branch: logfreq += detune>7 ? 13457*(detune-7) : 0
    if (detune_raw > 7) {
        const float detune_oct = 13457.0f * (detune_raw - 7) / 16777216.0f;
        hz *= std::pow(2.0f, detune_oct);
    }

    return hz;
}

} // namespace augr::fm