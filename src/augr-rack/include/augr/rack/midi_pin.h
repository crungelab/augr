#pragma once

#include <augr/core/midi/midi_message.h>
#include <augr/rack/pin.h>

namespace augr {

using MidiOutput = OutputT<MidiMessage, Pin>;
//using MidiInput = MonoInputT<MidiMessage, Pin>;
using MidiInput = QueueInputT<MidiMessage, Pin>;

} // namespace augr