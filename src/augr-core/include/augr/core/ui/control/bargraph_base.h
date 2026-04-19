#pragma once

#include "parameter_control.h"

namespace augr {

class BarGraphBase : public ParameterControl {
public:
    BarGraphBase(std::string label, Parameter* param)
        : ParameterControl(label, param) {}

    REFLECT_ENABLE(ParameterControl)
};

} // namespace augr