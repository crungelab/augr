#pragma once
#include "button_base.h"

namespace augr {

class CheckButton : public ButtonBase {
public:
  CheckButton(const char* label, fy_real* zone) : ButtonBase(label, zone) {}

  REFLECT_ENABLE(ButtonBase)
};

} // namespace augr