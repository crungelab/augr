#pragma once
#include "button_base.h"

namespace augr {

class Button : public ButtonBase {
public:
  Button(std::string label, BindingPtr zone) : ButtonBase(label, zone) {}

  REFLECT_ENABLE(ButtonBase)
};

} // namespace augr