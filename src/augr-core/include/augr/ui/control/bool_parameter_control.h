#pragma once

#include <augr/ui/control/bool_parameter.h>
#include <augr/ui/control/parameter_control.h>

namespace augr {

class BoolParameterControl : public ParameterControl<BoolParameter> {
public:
    BoolParameterControl() = default;
    explicit BoolParameterControl(std::string label, BoolParameter *param)
        : ParameterControl(std::move(label), param) {}

    REFLECT_ENABLE(ParameterControl<BoolParameter>)
};

} // namespace augr