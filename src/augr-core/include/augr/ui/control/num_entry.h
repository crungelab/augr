#pragma once
#include "range_base.h"

namespace augr {

class NumEntry : public RangeBase {
public:
    NumEntry(std::string label, FloatParameter *param) :
      RangeBase(label, param) {}

  REFLECT_ENABLE(RangeBase)
};

} // namespace augr