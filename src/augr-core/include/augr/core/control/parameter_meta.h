#ifndef AUGR_CORE_PARAMETER_META_H_
#define AUGR_CORE_PARAMETER_META_H_

#include <cstring>

#include <augr/core/control/parameter_unit.h>
#include <augr/core/meta.h>

namespace augr {

struct ParameterMeta : Meta {
    ParameterUnit Unit() const {
        const char *u = get("unit");
        if (!u)
            return ParameterUnit::kNone;
        if (strcmp(u, "dB") == 0)
            return ParameterUnit::kDecibel;
        if (strcmp(u, "Hz") == 0)
            return ParameterUnit::kHertz;
        if (strcmp(u, "kHz") == 0)
            return ParameterUnit::kKilohertz;
        if (strcmp(u, "s") == 0)
            return ParameterUnit::kSeconds;
        if (strcmp(u, "ms") == 0)
            return ParameterUnit::kMilliseconds;
        if (strcmp(u, "deg") == 0)
            return ParameterUnit::kDegrees;
        if (strcmp(u, "rad") == 0)
            return ParameterUnit::kRadians;
        if (strcmp(u, "%") == 0)
            return ParameterUnit::kPercent;
        if (strcmp(u, "st") == 0)
            return ParameterUnit::kSemitones;
        if (strcmp(u, "oct") == 0)
            return ParameterUnit::kOctaves;
        if (strcmp(u, "bpm") == 0)
            return ParameterUnit::kBpm;
        return ParameterUnit::kNone;
    }

    bool IsKnob() const {
        auto it = find("style");
        return it != end() && it->second == "knob";
    }
};

} // namespace augr

#endif // AUGR_CORE_PARAMETER_META_H_