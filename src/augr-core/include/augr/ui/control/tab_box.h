#pragma once
#include "box_base.h"

namespace augr {

class TabBox : public BoxBase {
public:
    TabBox(std::string label) : BoxBase(label) {}

    REFLECT_ENABLE(BoxBase)
};

} // namespace augr