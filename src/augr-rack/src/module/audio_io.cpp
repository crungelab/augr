#include <augr/core/archiver_factory.h>
#include <augr/core/model_factory.h>

#include <augr/rack/archiver/module_archiver.h>
#include <augr/rack/module/audio_io.h>

#include <augr/rack/graph.h>

namespace augr {

void AudioInputModule::Create(Model *parent) {
    Io::Create(parent);
    label_ = "Audio Input Module";
}

void AudioInputModule::CreatePins() {
    audio_out_ = new AudioOutput(*this, "audio_out", ChannelLayout::kStereo);
    AddOutput(*audio_out_);

    audio_in_ = new AudioInput(*this, "audio_in", ChannelLayout::kStereo);
    graph().AddInput(*audio_in_);
}

void AudioInputModule::Process() {
    Audio audio = audio_in_->Read();
    audio_out_->Write(audio);
}

void AudioOutputModule::Create(Model *parent) {
    Io::Create(parent);
    label_ = "Audio Output Module";
}

void AudioOutputModule::CreatePins() {
    audio_in_ = new AudioInput(*this, "audio_in", ChannelLayout::kStereo);
    AddInput(*audio_in_);

    audio_out_ = new AudioOutput(graph(), "audio_out", ChannelLayout::kStereo);
    graph().AddOutput(*audio_out_);
}

void AudioOutputModule::Process() {
    Audio audio = audio_in_->Read();
    audio_out_->Write(audio);
}

} // namespace augr

using namespace augr;

DEFINE_MODULE(AudioInputModule, "AudioInputModule", "Io")
DEFINE_MODULE(AudioOutputModule, "AudioOutputModule", "Io")
