#pragma once

#include <memory>
#include <string>
#include <vector>

#include <augr/binding.h>
#include <augr/config.h>
#include <augr/model.h>

#include <augr/ui/control/control.h>
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