#include <augr/rack/module/audio_device.h>

namespace augr {

bool AudioInputDevice::Create(Part &owner)
{
  Device::Create(owner);
  label_ = "Audio Input Device";
  return true;
}

void AudioInputDevice::CreatePins()
{
  audio_out_ = new AudioOutput(*this, "audio_out_", ChannelLayout::kStereo);
  AddOutput(*audio_out_);
}

bool AudioOutputDevice::Create(Part &owner)
{
  Device::Create(owner);
  label_ = "Audio Output Device";
  return true;
}

void AudioOutputDevice::CreatePins()
{
  audio_in_ = new AudioInput(*this, "audio_in_", ChannelLayout::kStereo);
  AddInput(*audio_in_);
}

} // namespace augr