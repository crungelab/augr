#pragma once

#include <augr/core/meta.h>

namespace augr {

enum class ParameterUnit {
    kNone,
    // Amplitude / level
    kDecibel, // "dB"
    // Frequency
    kHertz,     // "Hz"
    kKilohertz, // "kHz"
    // Time
    kSeconds,      // "s"
    kMilliseconds, // "ms"
    // Angle / phase
    kDegrees, // "deg"
    kRadians, // "rad"
    // Normalized / dimensionless
    kPercent,   // "%"
    kSemitones, // "st"
    kOctaves,   // "oct"
    kBpm,       // "bpm"
};

} // namespace augr
