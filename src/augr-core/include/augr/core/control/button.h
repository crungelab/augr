#pragma once
#include "button_base.h"

namespace augr {

class Button : public ButtonBase {
public:
  Button(const char* label, fy_real* zone) : ButtonBase(label, zone) {}

  REFLECT_ENABLE(ButtonBase)
};

} // namespace augr