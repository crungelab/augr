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
    // dexie_env.h — change signature:
    void NoteOn(const int rates[4], const int levels[4], int output_level,
                int rate_scaling, int level_scaling, int velocity_scaling);
    // Gate release: move to the R4/L4 segment.
    void NoteOff();

    // Advance one sample; returns linear amplitude in [0, 1].
    float Tick();

    bool IsIdle() const { return ix_ >= 4; }

private:
    void Advance(int newix);
    static int ScaleOutLevel(int outlevel);

    int rates_[4] = {99, 50, 50, 50};
    int levels_[4] = {99, 75, 50, 0};

    int outlevel_ = 0; // scaleoutlevel(0..99) << 5
    int rate_scaling_ = 0;

    // State, in the DX7 "actuallevel << 16" log domain.
    float level_ = 0.0f;
    float targetlevel_ = 0.0f;
    float inc_ = 0.0f;
    int ix_ = 4; // start idle
    bool rising_ = false;
    bool down_ = false;

    float sr_ratio_ = 1.0f; // 44100 / sample_rate
};

} // namespace augr::fm