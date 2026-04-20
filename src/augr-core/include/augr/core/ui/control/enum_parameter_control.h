#pragma once

#include <memory>
#include <string>
#include <vector>

#include <augr/core/binding.h>
#include <augr/core/config.h>
#include <augr/core/model.h>

#include <augr/core/ui/control/control.h>
#include <augr/core/ui/control/enum_parameter.h>
#include <augr/core/ui/control/parameter_control.h>

namespace augr {

class EnumParameterControl : public ParameterControl<EnumParameter> {
public:
    EnumParameterControl() = default;
    explicit EnumParameterControl(std::string label, EnumParameter *param)
        : ParameterControl(std::move(label), param) {}

    REFLECT_ENABLE(ParameterControl<EnumParameter>)
};

} // namespace augr