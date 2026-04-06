#pragma once
#include "control.h"

namespace augr {

class ButtonBase : public Control {
public:
  ButtonBase(std::string label, fy_real* zone) : Control(label, zone) {}

  REFLECT_ENABLE(Control)
};

} // namespace augr