// augr/fm/dexie_env.cpp
#include <augr/fm/dexie_env.h>

#include <algorithm>
#include <cmath>

namespace augr::fm {

namespace {

// DX7 output-level scaling for the compressed low end (levels 0..19); above
// 19 the curve is linear (28 + level). Straight from Env::scaleoutlevel.
const int kLevelLut[20] = {
    0, 5, 9, 13, 17, 20, 23, 25, 27, 29,
    31, 33, 35, 37, 39, 41, 42, 43, 45, 46
};

// "actuallevel" (level_ >> 16) value that maps to unity gain. A full carrier
// (output level 99, segment level 99) reaches 3840 in this domain:
//   (scaleoutlevel(99)>>1 << 6) + (scaleoutlevel(99)<<5) - 4256
//   = 4032 + 4064 - 4256 = 3840.
constexpr float kLevelMax = 3840.0f;

// DX7 log-level resolution: ~256 actuallevel units per octave (~6 dB),
// derived from the engine's level_in -> operator-gain mapping.
//constexpr float kLevelPerOctave = 256.0f;
//constexpr float kLevelPerOctave = 512.0f;
constexpr float kLevelPerOctave = 341.333333333f;

} // namespace

int DexieEnv::ScaleOutLevel(int outlevel) {
    if (outlevel < 0) outlevel = 0;
    if (outlevel > 99) outlevel = 99;
    return outlevel >= 20 ? 28 + outlevel : kLevelLut[outlevel];
}

void DexieEnv::SetSampleRate(float sample_rate) {
    sr_ratio_ = 44100.0f / sample_rate;
}

// dexie_env.cpp
void DexieEnv::NoteOn(const float rates[4], const float levels[4],
                      float output_level, int rate_scaling,
                      int level_scaling, int velocity_scaling) {
    for (int i = 0; i < 4; ++i) { rates_[i] = rates[i]; levels_[i] = levels[i]; }
    int ol = ScaleOutLevel(static_cast<int>(output_level)) + level_scaling;
    ol = std::clamp(ol, 0, 127);
    ol = (ol << 5) + velocity_scaling;
    ol = std::max(ol, 0);
    outlevel_     = ol;
    rate_scaling_ = rate_scaling;
    level_ = 0.0f;
    down_  = true;
    Advance(0);
}

void DexieEnv::NoteOff() {
    if (down_) {
        down_ = false;
        Advance(3);
    }
}

void DexieEnv::Advance(int newix) {
    ix_ = newix;
    if (ix_ < 4) {
        const int newlevel = static_cast<int>(levels_[ix_]);
        int actuallevel = ScaleOutLevel(newlevel) >> 1;
        actuallevel = (actuallevel << 6) + outlevel_ - 4256;
        actuallevel = actuallevel < 16 ? 16 : actuallevel;
        targetlevel_ = static_cast<float>(actuallevel) * 65536.0f;  // << 16
        rising_ = targetlevel_ > level_;

        int qrate = (static_cast<int>(rates_[ix_]) * 41) >> 6;
        qrate += rate_scaling_;
        qrate = std::min(qrate, 63);

        // Per-sample increment. Dexed's per-block shift is
        // (2 + LG_N + (qrate>>2)) with LG_N = 6 (64-sample block); dropping
        // LG_N yields the per-sample rate (i.e. divide the block step by 64).
        inc_ = static_cast<float>((4 + (qrate & 3)) << (2 + (qrate >> 2)));
        inc_ *= sr_ratio_;
    }
}

float DexieEnv::Tick() {
    // Segments 0..2 always advance; segment 3 (release) only advances once the
    // gate is up. ix_ == 4 holds the final level.
    if (ix_ < 3 || (ix_ < 4 && !down_)) {
        if (rising_) {
            constexpr float kJumpTarget = 1716.0f * 65536.0f;   // 1716 << 16
            if (level_ < kJumpTarget) level_ = kJumpTarget;
            constexpr float k17_24 = 17.0f * 16777216.0f;       // 17 << 24
            level_ += ((k17_24 - level_) / 16777216.0f) * inc_;
            if (level_ >= targetlevel_) {
                level_ = targetlevel_;
                Advance(ix_ + 1);
            }
        } else {
            level_ -= inc_;
            if (level_ <= targetlevel_) {
                level_ = targetlevel_;
                Advance(ix_ + 1);
            }
        }
    }

    const float actuallevel = level_ / 65536.0f;
    const float amp = std::exp2((actuallevel - kLevelMax) / kLevelPerOctave);
    return std::clamp(amp, 0.0f, 1.0f);
}

} // namespace augr::fm