#pragma once
#include "slider_base.h"

namespace augr {

class VSlider : public SliderBase {
public:
    VSlider(std::string label, FloatParameter *param)
        : SliderBase(label, param) {}

    REFLECT_ENABLE(SliderBase)
};

} // namespace augr