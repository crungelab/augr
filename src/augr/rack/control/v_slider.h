#pragma once
#include "slider_base.h"

namespace augr {

class VSlider : public SliderBase {
public:
  VSlider(const char* label, fy_real* zone, fy_real init, fy_real min, fy_real max, fy_real step) :
    SliderBase(label, zone, init, min, max, step) {}

  REFLECT_ENABLE(SliderBase)
};

} // namespace augr