// augr/fm/dexie_env.h
#pragma once

namespace augr::fm {

// A faithful float reimplementation of the DX7 envelope generator, ported
// from Dexed's Mark I engine (env.cc). The envelope runs in the DX7's
// logarithmic "actuallevel" domain — segment targets combine the per-segment
// level (L1..L4, via scaleoutlevel) additively with the operator output
// level, the level ramps linearly *in log space* (which produces the
// characteristic DX7 envelope shape), and the final log level is converted to
// a linear amplitude on output.
//
// Segment mapping: ix 0 = R1/L1 (attack), 1 = R2/L2, 2 = R3/L3 (the held
// sustain level), 3 = R4/L4 (release). Sustain holds at L3 while the gate is
// down; NoteOff jumps to the R4/L4 release segment.
class DexieEnv {
public:
    void SetSampleRate(float sample_rate);

    // Note-on: (re)start from zero. rates/levels are raw DX7 0..99 values;
    // output_level is the raw operator output level 0..99; rate_scaling is the
    // pitch-derived rate offset (0 when unused).
    void NoteOn(const float rates[4], const float levels[4],
                float output_level, int rate_scaling);

    // Gate release: move to the R4/L4 segment.
    void NoteOff();

    // Advance one sample; returns linear amplitude in [0, 1].
    float Tick();

    bool IsIdle() const { return ix_ >= 4; }

private:
    void Advance(int newix);
    static int ScaleOutLevel(int outlevel);

    float rates_[4]     = { 0.f, 0.f, 0.f, 0.f };
    float levels_[4]    = { 0.f, 0.f, 0.f, 0.f };
    int   outlevel_     = 0;       // scaleoutlevel(0..99) << 5
    int   rate_scaling_ = 0;

    // State, in the DX7 "actuallevel << 16" log domain.
    float level_       = 0.0f;
    float targetlevel_ = 0.0f;
    float inc_         = 0.0f;
    int   ix_          = 4;        // start idle
    bool  rising_      = false;
    bool  down_        = false;

    float sr_ratio_ = 1.0f;        // 44100 / sample_rate
};

} // namespace augr::fm