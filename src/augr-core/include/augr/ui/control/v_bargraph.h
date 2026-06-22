#pragma once
#include "bargraph_base.h"

namespace augr {

class VBarGraph : public BarGraphBase {
public:
    VBarGraph(std::string label, FloatParameter *param)
        : BarGraphBase(label, param) {}

    REFLECT_ENABLE(BarGraphBase)
};

} // namespace augr