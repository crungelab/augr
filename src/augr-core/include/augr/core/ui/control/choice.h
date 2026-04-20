#pragma once

#include "control.h"

namespace augr {

class Choice : public IntControl {
public:
    Choice(std::string label, ControlMeta meta, BindingPtr binding, int precision)
        : IntControl(label, meta, binding), precision_(precision) {}
    // Data members
    int precision_;

    REFLECT_ENABLE(Control)
};

} // namespace augr