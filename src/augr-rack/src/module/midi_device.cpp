
#include <augr/core/model_factory.h>
#include <augr/core/archiver_factory.h>

#include <augr/rack/module/midi_device.h>
#include <augr/rack/archiver/module_archiver.h>

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

using namespace augr;
DEFINE_MODEL_FACTORY(MidiInputDevice, "MidiInputDevice", "")
DEFINE_MODEL_FACTORY(MidiOutputDevice, "MidiOutputDevice", "")

class MidiInputDeviceArchiver : public ModuleArchiver {};
DEFINE_ARCHIVER_FACTORY(MidiInputDeviceArchiver, MidiInputDevice,
                        "MidiInputDevice")

class MidiOutputDeviceArchiver : public ModuleArchiver {};
DEFINE_ARCHIVER_FACTORY(MidiOutputDeviceArchiver, MidiOutputDevice,
                        "MidiOutputDevice")
