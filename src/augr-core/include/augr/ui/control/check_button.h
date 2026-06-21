#pragma once
#include "float_parameter_control.h"
#include "bool_parameter_control.h"

namespace augr {

class CheckButton : public FloatParameterControl {
public:
  CheckButton(std::string label, FloatParameter *param) : FloatParameterControl(label, param) {}

  REFLECT_ENABLE(FloatParameterControl)
};

class CheckButtonBool : public BoolParameterControl {
public:
  CheckButtonBool(std::string label, BoolParameter *param) : BoolParameterControl(label, param) {}

  REFLECT_ENABLE(BoolParameterControl)
};

} // namespace augr