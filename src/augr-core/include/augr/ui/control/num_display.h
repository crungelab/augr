#pragma once

#include "control.h"

namespace augr {

class NumDisplay : public FloatControl {
public:
    NumDisplay(std::string label, ControlMeta meta, BindingPtr binding, int precision)
        : FloatControl(label, meta, binding), precision_(precision) {}
    // Data members
    int precision_;

    REFLECT_ENABLE(Control)
};

} // namespace augr