#pragma once
#include "bargraph_base.h"

namespace augr {

class HBarGraph : public BarGraphBase {
public:
    HBarGraph(std::string label, Parameter *param, fy_real min, fy_real max)
        : BarGraphBase(label, param, min, max) {}

    REFLECT_ENABLE(BarGraphBase)
};

} // namespace augr