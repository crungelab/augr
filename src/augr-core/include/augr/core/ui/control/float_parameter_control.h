#pragma once

#include <memory>
#include <string>
#include <vector>

#include <augr/core/binding.h>
#include <augr/core/config.h>
#include <augr/core/model.h>

#include <augr/core/ui/control/control.h>
#include <augr/core/ui/control/float_parameter.h>
#include <augr/core/ui/control/parameter_control.h>

namespace augr {

class FloatParameterControl : public ParameterControl<FloatParameter> {
public:
    FloatParameterControl() = default;
    explicit FloatParameterControl(std::string label, FloatParameter *param)
        : ParameterControl(std::move(label), param) {}

    REFLECT_ENABLE(ParameterControl<FloatParameter>)
};

} // namespace augr