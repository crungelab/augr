#pragma once
#include "box_base.h"

namespace augr {

class FrameBox : public BoxBase {
public:
    FrameBox(std::string label) : BoxBase(label) {}

    REFLECT_ENABLE(BoxBase)
};

} // namespace augr