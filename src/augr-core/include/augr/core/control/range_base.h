#pragma once

#include "parameter_control.h"

namespace augr {

class RangeBase : public ParameterControl {
public:
    RangeBase(std::string label, Parameter *param)
        : ParameterControl(label, param) {}
    // Data members

    REFLECT_ENABLE(ParameterControl)
};

} // namespace augr