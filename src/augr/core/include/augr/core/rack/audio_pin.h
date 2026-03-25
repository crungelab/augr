#pragma once

#include <augr/core/audio.h>
#include <augr/core/rack/pin.h>

namespace augr {

typedef PinT<Audio> AudioPinBase;

class AudioPin : public AudioPinBase
{
public:
  AudioPin(Node &node, std::string name, ChannelLayout layout = ChannelLayout::kMono) : AudioPinBase(node, name), layout_(layout)
  {
  }
  void Write(Audio audio) override
  {
    if(audio.layout_ != layout_) {
      //audio = audio.Convert(format_);
      AudioPinBase::Write(audio.Convert(layout_));
      return;
    }
    AudioPinBase::Write(audio);
  }
  // Data members
  ChannelLayout layout_ = ChannelLayout::kMono;
};

} // namespace augr