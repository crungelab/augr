#pragma once
#include "range_base.h"

namespace augr {

class SliderBase : public RangeBase {
public:
    SliderBase(std::string label, FloatParameter *param)
        : RangeBase(label, param) {}

    REFLECT_ENABLE(RangeBase)
};

} // namespace augr