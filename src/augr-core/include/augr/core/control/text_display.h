#pragma once
#include "control.h"

namespace augr {

class TextDisplay : public Control {
public:
  TextDisplay(const char* label, fy_real* zone, char* names[], fy_real min, fy_real max) :
    Control(label,  zone), names_(names), min_(min), max_(max) {}
  //Data members
  char** names_;
  fy_real min_, max_;

  REFLECT_ENABLE(Control)
};

} // namespace augr