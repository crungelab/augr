#pragma once

#include "enum_parameter_control.h"

namespace augr {

class Combo : public EnumParameterControl {
public:
    Combo(const std::string &label, EnumParameter *param)
        : EnumParameterControl(label, param) {}
    // Data members

    REFLECT_ENABLE(EnumParameterControl)
};

} // namespace augr