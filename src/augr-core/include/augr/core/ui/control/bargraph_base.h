#pragma once

#include "float_parameter_control.h"

namespace augr {

class BarGraphBase : public FloatParameterControl {
public:
    BarGraphBase(std::string label, FloatParameter* param)
        : FloatParameterControl(label, param) {}

    REFLECT_ENABLE(FloatParameterControl)
};

} // namespace augr