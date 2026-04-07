#pragma once
#include "control.h"

namespace augr {

class ButtonBase : public FloatControl {
public:
  ButtonBase(std::string label, BindingPtr zone) : FloatControl(label, zone) {}

  REFLECT_ENABLE(Control)
};

} // namespace augr