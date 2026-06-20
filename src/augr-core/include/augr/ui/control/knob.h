#pragma once
#include "float_parameter_control.h"
#include "int_parameter_control.h"

namespace augr {

class Knob : public FloatParameterControl {
public:
    Knob(std::string label, FloatParameter *param) : FloatParameterControl(label, param) {}

    REFLECT_ENABLE(FloatParameterControl)
};

class KnobInt : public IntParameterControl {
public:
    KnobInt(std::string label, IntParameter *param) : IntParameterControl(label, param) {}
    REFLECT_ENABLE(IntParameterControl)
};

} // namespace augr