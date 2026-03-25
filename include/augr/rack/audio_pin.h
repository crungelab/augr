#pragma once

#include <augr/audio.h>
#include <augr/rack/pin.h>

namespace augr {

typedef PinT<Audio> AudioPinBase;

class AudioPin : public AudioPinBase
{
public:
  AudioPin(Node &node, std::string name, Audio::Format format = Audio::Format::kMono) : AudioPinBase(node, name), format_(format)
  {
  }
  void Write(Audio audio) override
  {
    if(audio.format_ != format_) {
      //audio = audio.Convert(format_);
      AudioPinBase::Write(audio.Convert(format_));
      return;
    }
    AudioPinBase::Write(audio);
  }
  // Data members
  Audio::Format format_ = Audio::Format::kMono;
};

} // namespace augr