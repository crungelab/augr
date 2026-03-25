#pragma once
#include "range_base.h"

namespace augr {

class SliderBase : public RangeBase {
public:
  SliderBase(const char* label, fy_real* zone, fy_real init, fy_real min, fy_real max, fy_real step) :
    RangeBase(label, zone, init, min, max, step) {}

  REFLECT_ENABLE(RangeBase)
};

} // namespace augr