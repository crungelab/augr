#pragma once
#include "control.h"

namespace augr {

class RangeBase : public ParameterControl {
public:
    RangeBase(std::string label, Parameter *param)
        : ParameterControl(label, param) {}
    // Data members

    REFLECT_ENABLE(ParameterControl)
};

/*
class RangeBase : public ParameterControl {
public:
    RangeBase(std::string label, Parameter *param, fy_real init, fy_real min,
              fy_real max, fy_real step)
        : ParameterControl(label, param), init_(init), min_(min), max_(max),
          step_(step) {}
    // Data members
    fy_real init_;
    fy_real min_;
    fy_real max_;
    fy_real step_;

    REFLECT_ENABLE(Control)
};
*/

} // namespace augr