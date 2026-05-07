#include <augr/rack/module/audio_device.h>

namespace augr {

void AudioInputDevice::Create(Part *owner)
{
  Device::Create(owner);
  label_ = "Audio Input Device";
}

void AudioInputDevice::CreatePins()
{
  audio_out_ = new AudioOutput(*this, "audio_out", ChannelLayout::kStereo);
  AddOutput(*audio_out_);
}

void AudioOutputDevice::Create(Part *owner)
{
  Device::Create(owner);
  label_ = "Audio Output Device";
}

void AudioOutputDevice::CreatePins()
{
  audio_in_ = new AudioInput(*this, "audio_in", ChannelLayout::kStereo);
  AddInput(*audio_in_);
}

} // namespace augr