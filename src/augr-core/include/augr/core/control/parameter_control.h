#pragma once

#include <memory>
#include <string>
#include <vector>

#include <augr/core/binding.h>
#include <augr/core/config.h>
#include <augr/core/model.h>

#include <augr/core/control/control.h>
#include <augr/core/control/parameter.h>

namespace augr {

class ParameterControl : public Control {
public:
    ParameterControl() = default;
    explicit ParameterControl(std::string label, Parameter *param)
        : Control(std::move(label), param->meta()), param_(param) {}

    Parameter *param() { return param_; }
    const Parameter *param() const { return param_; }

    REFLECT_ENABLE(Control)

private:
    Parameter *param_; // non-owning — Parameter lives in Module
};

} // namespace augr