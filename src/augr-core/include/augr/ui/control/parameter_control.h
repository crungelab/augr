#pragma once

#include <string>

#include <augr/ui/control/control.h>

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