#pragma once
#include "control.h"

namespace augr {

class TextDisplay : public FloatControl {
public:
  TextDisplay(std::string label, PropertyPtr zone, char* names[], fy_real min, fy_real max) :
    FloatControl(label,  zone), names_(names), min_(min), max_(max) {}
  //Data members
  char** names_;
  fy_real min_, max_;

  REFLECT_ENABLE(Control)
};

} // namespace augr