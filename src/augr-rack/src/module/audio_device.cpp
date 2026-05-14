#include <augr/core/model_factory.h>
#include <augr/core/archiver_factory.h>

#include <augr/rack/module/audio_device.h>
#include <augr/rack/archiver/module_archiver.h>

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

using namespace augr;

DEFINE_MODEL_FACTORY(AudioInputDevice, "AudioInputDevice", "")
DEFINE_MODEL_FACTORY(AudioOutputDevice, "AudioOutputDevice", "")

class AudioInputDeviceArchiver : public ModuleArchiver {};
DEFINE_ARCHIVER_FACTORY(AudioInputDeviceArchiver, AudioInputDevice,
                        "AudioInputDevice")

class AudioOutputDeviceArchiver : public ModuleArchiver {};
DEFINE_ARCHIVER_FACTORY(AudioOutputDeviceArchiver, AudioOutputDevice,
                        "AudioOutputDevice")
