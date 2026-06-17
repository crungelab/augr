#pragma once
#include "range_base.h"

namespace augr {

class SliderBase : public RangeBase {
public:
    SliderBase(std::string label, FloatParameter *param)
        : RangeBase(label, param) {}

    REFLECT_ENABLE(RangeBase)
};

/*
class SliderBase : public RangeBase {
public:
    SliderBase(std::string label, FloatParameter *param, fy_real init, fy_real min,
               fy_real max, fy_real step)
        : RangeBase(label, param, init, min, max, step) {}

    REFLECT_ENABLE(RangeBase)
};
*/

} // namespace augr