#pragma once

#include <augr/ui/control/int_parameter.h>
#include <augr/ui/control/parameter_control.h>

namespace augr {

class IntParameterControl : public ParameterControl<IntParameter> {
public:
    IntParameterControl() = default;
    explicit IntParameterControl(std::string label, IntParameter *param)
        : ParameterControl(std::move(label), param) {}

    REFLECT_ENABLE(ParameterControl<IntParameter>)
};

} // namespace augr