#include <augr/rack/module/midi_device.h>

namespace augr {

bool MidiInputDevice::Create(Part &owner)
{
  Device::Create(owner);
  label_ = "Midi Input Device";
  midi_out_ = new MidiOutput(*this, "midi_out_");
  AddOutput(*midi_out_);
  return true;
}

bool MidiOutputDevice::Create(Part &owner)
{
  Device::Create(owner);
  label_ = "Midi Output Device";
  midi_in_ = new MidiInput(*this, "midi_in_");
  AddInput(*midi_in_);
  return true;
}

} // namespace augr