#pragma once

#include <augr/audio.h>

#include "port.h"

namespace augr {

class AudioOutport : public Outport {
public:
  virtual void CreatePin(std::string name) {
    //AddPin(new )
  }
  void Write(Audio audio) {
    audio_ = audio;
    for(auto pin : pins_) {

    }
  }
  //Data members
  Audio audio_;
};

} // namespace augr