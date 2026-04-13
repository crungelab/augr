#pragma once

#include <augr/core/meta.h>

namespace augr {

enum class ControlUnit {
    None,
    // Amplitude / level
    Decibel, // "dB"
    // Frequency
    Hertz,     // "Hz"
    Kilohertz, // "kHz"
    // Time
    Seconds,      // "s"
    Milliseconds, // "ms"
    // Angle / phase
    Degrees, // "deg"
    Radians, // "rad"
    // Normalized / dimensionless
    Percent,   // "%"
    Semitones, // "st"
    Octaves,   // "oct"
    BPM,       // "bpm"
};

} // namespace augr