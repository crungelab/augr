#pragma once
#include "control.h"

namespace augr {

class BoxBase : public Control {
public:
  BoxBase(const char* label) : Control(label) {}
  //Data members
  bool is_top_level_ = false;
  bool label_visible_ = true;

  REFLECT_ENABLE(Control)
};

} // namespace augr