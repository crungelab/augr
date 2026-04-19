#ifndef AUGR_CORE_PARAMETER_META_H_
#define AUGR_CORE_PARAMETER_META_H_

#include <cstring>

#include <augr/core/control/control_unit.h>
#include <augr/core/meta.h>

namespace augr {

struct ControlMeta : Meta {
    ControlUnit Unit() const {
        const char *u = get("unit");
        if (!u)
            return ControlUnit::kNone;
        if (strcmp(u, "dB") == 0)
            return ControlUnit::kDecibel;
        if (strcmp(u, "Hz") == 0)
            return ControlUnit::kHertz;
        if (strcmp(u, "kHz") == 0)
            return ControlUnit::kKilohertz;
        if (strcmp(u, "s") == 0)
            return ControlUnit::kSeconds;
        if (strcmp(u, "ms") == 0)
            return ControlUnit::kMilliseconds;
        if (strcmp(u, "deg") == 0)
            return ControlUnit::kDegrees;
        if (strcmp(u, "rad") == 0)
            return ControlUnit::kRadians;
        if (strcmp(u, "%") == 0)
            return ControlUnit::kPercent;
        if (strcmp(u, "st") == 0)
            return ControlUnit::kSemitones;
        if (strcmp(u, "oct") == 0)
            return ControlUnit::kOctaves;
        if (strcmp(u, "bpm") == 0)
            return ControlUnit::kBpm;
        return ControlUnit::kNone;
    }

    bool IsKnob() const {
        auto it = find("style");
        return it != end() && it->second == "knob";
    }
};

} // namespace augr

#endif // AUGR_CORE_PARAMETER_META_H_