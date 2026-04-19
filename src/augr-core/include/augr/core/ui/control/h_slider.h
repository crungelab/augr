#pragma once
#include "slider_base.h"

namespace augr {

class HSlider : public SliderBase {
public:
  HSlider(std::string label, Parameter *param) :
    SliderBase(label, param) {}

  REFLECT_ENABLE(SliderBase)
};

} // namespace augr