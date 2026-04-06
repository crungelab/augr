#pragma once
#include "button_base.h"

namespace augr {

class Button : public ButtonBase {
public:
  Button(std::string label, PropertyPtr zone) : ButtonBase(label, zone) {}

  REFLECT_ENABLE(ButtonBase)
};

} // namespace augr