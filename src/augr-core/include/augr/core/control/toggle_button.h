#pragma once
#include "button_base.h"

namespace augr {

class ToggleButton : public ButtonBase {
public:
  ToggleButton(std::string label, PropertyPtr zone) : ButtonBase(label, zone) {}

  REFLECT_ENABLE(ButtonBase)
};

} // namespace augr