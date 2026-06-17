#pragma once
#include "button_base.h"

namespace augr {

class CheckButton : public ButtonBase {
public:
  CheckButton(std::string label, FloatParameter *param) : ButtonBase(label, param) {}

  REFLECT_ENABLE(ButtonBase)
};

} // namespace augr