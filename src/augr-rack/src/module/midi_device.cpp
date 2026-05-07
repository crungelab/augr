#include <augr/rack/module/midi_device.h>

namespace augr {

void MidiInputDevice::Create(Part *owner) {
    Device::Create(owner);
    label_ = "Midi Input Device";
}

void MidiInputDevice::CreatePins() {
    midi_out_ = new MidiOutput(*this, "midi_out_");
    AddOutput(*midi_out_);
}

void MidiOutputDevice::Create(Part *owner) {
    Device::Create(owner);
    label_ = "Midi Output Device";
}

void MidiOutputDevice::CreatePins() {
    midi_in_ = new MidiInput(*this, "midi_in_");
    AddInput(*midi_in_);
}

} // namespace augr