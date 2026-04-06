#pragma once
#include "button_base.h"

namespace augr {

class CheckButton : public ButtonBase {
public:
  CheckButton(std::string label, PropertyPtr zone) : ButtonBase(label, zone) {}

  REFLECT_ENABLE(ButtonBase)
};

} // namespace augr