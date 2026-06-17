#include <augr/archiver_factory.h>
#include <augr/model_factory.h>

#include <augr/rack/archiver/module_archiver.h>
#include <augr/rack/module/audio_device.h>

namespace augr {

void AudioInputDevice::OnCreate() {
    Device::OnCreate();
    label_ = "Audio Input Device";
}

void AudioInputDevice::CreatePins() {
    audio_out_ = new AudioOutput(*this, "audio_out", ChannelLayout::kStereo);
    AddOutput(*audio_out_);
}

void AudioOutputDevice::OnCreate() {
    Device::OnCreate();
    label_ = "Audio Output Device";
}

void AudioOutputDevice::CreatePins() {
    audio_in_ = new AudioInput(*this, "audio_in", ChannelLayout::kStereo);
    AddInput(*audio_in_);
}

} // namespace augr

using namespace augr;

DEFINE_MODULE(AudioInputDevice, "AudioInputDevice", "")
DEFINE_MODULE(AudioOutputDevice, "AudioOutputDevice", "")
