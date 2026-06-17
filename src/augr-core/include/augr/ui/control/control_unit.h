#pragma once

#include <augr/meta.h>

namespace augr {

enum class ControlUnit {
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
