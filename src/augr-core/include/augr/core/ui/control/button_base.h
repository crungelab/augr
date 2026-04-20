#pragma once

#include "float_parameter_control.h"

namespace augr {

class ButtonBase : public FloatParameterControl {
public:
  ButtonBase(std::string label, FloatParameter* param) : FloatParameterControl(label, param) {}

  REFLECT_ENABLE(FloatParameterControl)
};

} // namespace augr