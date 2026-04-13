#pragma once
#include "bargraph_base.h"

namespace augr {

class VBarGraph : public BarGraphBase {
public:
  VBarGraph(std::string label, Parameter *param, fy_real min, fy_real max) : BarGraphBase(label, param, min, max) {}

  REFLECT_ENABLE(BarGraphBase)
};

} // namespace augr