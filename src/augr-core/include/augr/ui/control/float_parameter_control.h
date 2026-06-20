#pragma once

#include <augr/ui/control/float_parameter.h>
#include <augr/ui/control/parameter_control.h>

namespace augr {

class FloatParameterControl : public ParameterControl<FloatParameter> {
public:
    FloatParameterControl() = default;
    explicit FloatParameterControl(std::string label, FloatParameter *param)
        : ParameterControl(std::move(label), param) {}

    REFLECT_ENABLE(ParameterControl<FloatParameter>)
};

} // namespace augr