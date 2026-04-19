#pragma once
#include "button_base.h"

namespace augr {

class ToggleButton : public ButtonBase {
public:
  ToggleButton(std::string label, Parameter *param) : ButtonBase(label, param) {}

  REFLECT_ENABLE(ButtonBase)
};

} // namespace augr