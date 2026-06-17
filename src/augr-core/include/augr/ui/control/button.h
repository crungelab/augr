#pragma once
#include "button_base.h"

namespace augr {

class Button : public ButtonBase {
public:
  Button(std::string label, FloatParameter *param) : ButtonBase(label, param) {}

  REFLECT_ENABLE(ButtonBase)
};

} // namespace augr