#pragma once
#include "bargraph_base.h"

namespace augr {

class VBarGraph : public BarGraphBase {
public:
  VBarGraph(std::string label, BindingPtr zone, fy_real min, fy_real max) : BarGraphBase(label, zone, min, max) {}

  REFLECT_ENABLE(BarGraphBase)
};

} // namespace augr