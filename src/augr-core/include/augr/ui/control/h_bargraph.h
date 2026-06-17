#pragma once
#include "bargraph_base.h"

namespace augr {

class HBarGraph : public BarGraphBase {
public:
    HBarGraph(std::string label, FloatParameter *param)
        : BarGraphBase(label, param) {}

    REFLECT_ENABLE(BarGraphBase)
};

} // namespace augr