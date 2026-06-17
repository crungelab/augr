#pragma once

#include <memory>
#include <string>
#include <vector>

#include <augr/binding.h>
#include <augr/config.h>
#include <augr/model.h>

#include <augr/ui/control/control.h>
#include <augr/ui/control/enum_parameter.h>
#include <augr/ui/control/parameter_control.h>

namespace augr {

class EnumParameterControl : public ParameterControl<EnumParameter> {
public:
    EnumParameterControl() = default;
    explicit EnumParameterControl(const std::string &label, EnumParameter *param)
        : ParameterControl(label, param) {}

    REFLECT_ENABLE(ParameterControl<EnumParameter>)
};

} // namespace augr