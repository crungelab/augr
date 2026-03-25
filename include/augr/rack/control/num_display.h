#pragma once
#include "control.h"

namespace augr {

class NumDisplay : public Control {
public:
  NumDisplay(const char* label, fy_real* zone, int precision) : Control(label, zone), precision_(precision) {}
  //Data members
  int precision_;

  REFLECT_ENABLE(Control)
};

} // namespace augr