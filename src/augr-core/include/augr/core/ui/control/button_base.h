#pragma once

#include "parameter_control.h"

namespace augr {

class ButtonBase : public ParameterControl {
public:
  ButtonBase(std::string label, Parameter* param) : ParameterControl(label, param) {}

  REFLECT_ENABLE(ParameterControl)
};

} // namespace augr