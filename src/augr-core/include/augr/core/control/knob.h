#pragma once
#include "range_base.h"

namespace augr {

class Knob : public RangeBase {
public:
  Knob(std::string label, BindingPtr zone, fy_real init, fy_real min, fy_real max, fy_real step) :
    RangeBase(label, zone, init, min, max, step) {}

  REFLECT_ENABLE(RangeBase)
};

} // namespace augr