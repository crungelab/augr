#pragma once
#include "control.h"

namespace augr {

class ButtonBase : public Control {
public:
  ButtonBase(const char* label, fy_real* zone) : Control(label, zone) {}

  REFLECT_ENABLE(Control)
};

} // namespace augr