#pragma once
#include "control.h"

namespace augr {

class NumDisplay : public FloatControl {
public:
  NumDisplay(std::string label, PropertyPtr zone, int precision) : FloatControl(label, zone), precision_(precision) {}
  //Data members
  int precision_;

  REFLECT_ENABLE(Control)
};

} // namespace augr