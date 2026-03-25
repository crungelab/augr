#pragma once
#include "box_base.h"

namespace augr {

class HBox : public BoxBase {
public:
  HBox(const char* label) : BoxBase(label) {}

  REFLECT_ENABLE(BoxBase)
};

} // namespace augr