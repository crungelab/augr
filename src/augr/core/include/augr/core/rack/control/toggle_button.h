#pragma once
#include "button_base.h"

namespace augr {

class ToggleButton : public ButtonBase {
public:
  ToggleButton(const char* label, fy_real* zone) : ButtonBase(label, zone) {}

  REFLECT_ENABLE(ButtonBase)
};

} // namespace augr