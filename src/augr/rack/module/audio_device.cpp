#include <augr/rack/module/audio_device.h>

namespace augr {

bool AudioInputDevice::Create(Part &owner)
{
  Device::Create(owner);
  label_ = "Audio Input Device";
  audio_out_ = new AudioPin(*this, "audio_out_", Audio::Format::kStereo);
  AddOutput(*audio_out_);
  return true;
}

bool AudioOutputDevice::Create(Part &owner)
{
  Device::Create(owner);
  label_ = "Audio Output Device";
  audio_in_ = new AudioPin(*this, "audio_in_", Audio::Format::kStereo);
  AddInput(*audio_in_);
  return true;
}

} // namespace augr