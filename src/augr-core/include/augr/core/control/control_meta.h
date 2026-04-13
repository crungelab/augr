#pragma once

#include <cstring>

#include <augr/core/meta.h>

#include <augr/core/control/control_unit.h>

namespace augr {

struct ControlMeta : Meta {
    ControlUnit unit() const {
        const char *u = get("unit");
        if (!u)
            return ControlUnit::None;
        if (strcmp(u, "dB") == 0)
            return ControlUnit::Decibel;
        if (strcmp(u, "Hz") == 0)
            return ControlUnit::Hertz;
        if (strcmp(u, "kHz") == 0)
            return ControlUnit::Kilohertz;
        if (strcmp(u, "s") == 0)
            return ControlUnit::Seconds;
        if (strcmp(u, "ms") == 0)
            return ControlUnit::Milliseconds;
        if (strcmp(u, "deg") == 0)
            return ControlUnit::Degrees;
        if (strcmp(u, "rad") == 0)
            return ControlUnit::Radians;
        if (strcmp(u, "%") == 0)
            return ControlUnit::Percent;
        if (strcmp(u, "st") == 0)
            return ControlUnit::Semitones;
        if (strcmp(u, "oct") == 0)
            return ControlUnit::Octaves;
        if (strcmp(u, "bpm") == 0)
            return ControlUnit::BPM;
        return ControlUnit::None;
    }

    bool isKnob() const {
        auto it = find("style");
        return it != end() && it->second == "knob";
    }
};

} // namespace augr