#pragma once
#include "range_base.h"

namespace augr {

class Knob : public RangeBase {
public:
    Knob(std::string label, Parameter *param) : RangeBase(label, param) {}

    REFLECT_ENABLE(RangeBase)
};

} // namespace augr