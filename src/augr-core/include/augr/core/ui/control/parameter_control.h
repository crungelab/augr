#pragma once

#include <memory>
#include <string>
#include <vector>

#include <augr/core/binding.h>
#include <augr/core/config.h>
#include <augr/core/model.h>

#include <augr/core/ui/control/control.h>
#include <augr/core/ui/control/float_parameter.h>

namespace augr {

template <typename T, typename TBase = Control>
class ParameterControl : public Control {
public:
    ParameterControl() = default;
    explicit ParameterControl(const std::string &label, T *param)
        : Control(label, param->meta()), param_(param) {}

    T *param() { return param_; }
    const T *param() const { return param_; }

    REFLECT_ENABLE(Control)

private:
    T *param_; // non-owning — Parameter lives in Module
};

} // namespace augr