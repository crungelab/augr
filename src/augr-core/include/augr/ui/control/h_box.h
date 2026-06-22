#pragma once
#include "box_base.h"

namespace augr {

class HBox : public BoxBase {
public:
    HBox(std::string label) : BoxBase(label) {}

    REFLECT_ENABLE(BoxBase)
};

} // namespace augr