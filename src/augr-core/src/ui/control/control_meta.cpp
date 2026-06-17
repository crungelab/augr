#include <augr/ui/control/control_meta.h>

namespace augr {

const ControlMeta ControlMeta::kDefault = [] {
    return ControlMeta();
}();

const ControlMeta ControlMeta::kDecibel = [] {
    ControlMeta m;
    m.declare("unit", "dB");
    return m;
}();

const ControlMeta ControlMeta::kHertz = [] {
    ControlMeta m;
    m.declare("unit", "Hz");
    return m;
}();

const ControlMeta ControlMeta::kKilohertz = [] {
    ControlMeta m;
    m.declare("unit", "kHz");
    return m;
}();

const ControlMeta ControlMeta::kSeconds = [] {
    ControlMeta m;
    m.declare("unit", "s");
    return m;
}();

const ControlMeta ControlMeta::kMilliseconds = [] {
    ControlMeta m;
    m.declare("unit", "ms");
    return m;
}();

const ControlMeta ControlMeta::kDegrees = [] {
    ControlMeta m;
    m.declare("unit", "deg");
    return m;
}();

const ControlMeta ControlMeta::kRadians = [] {
    ControlMeta m;
    m.declare("unit", "rad");
    return m;
}();

const ControlMeta ControlMeta::kPercent = [] {
    ControlMeta m;
    m.declare("unit", "%%");
    return m;
}();

const ControlMeta ControlMeta::kSemitones = [] {
    ControlMeta m;
    m.declare("unit", "st");
    return m;
}();

const ControlMeta ControlMeta::kOctaves = [] {
    ControlMeta m;
    m.declare("unit", "oct");
    return m;
}();

const ControlMeta ControlMeta::kBpm = [] {
    ControlMeta m;
    m.declare("unit", "bpm");
    return m;
}();

} // namespace augr