#pragma once

#include "float_parameter_control.h"

namespace augr {

class RangeBase : public FloatParameterControl {
public:
    RangeBase(std::string label, FloatParameter *param)
        : FloatParameterControl(label, param) {}
    // Data members

    REFLECT_ENABLE(FloatParameterControl)
};

} // namespace augr