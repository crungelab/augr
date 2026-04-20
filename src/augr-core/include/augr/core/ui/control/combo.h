#pragma once

#include "enum_parameter_control.h"

namespace augr {

class Choice : public EnumParameterControl {
public:
    Choice(std::string label, ControlMeta meta, EnumParameter *param)
        : EnumParameterControl(label, param) {}
    // Data members

    REFLECT_ENABLE(EnumParameterControl)
};

} // namespace augr