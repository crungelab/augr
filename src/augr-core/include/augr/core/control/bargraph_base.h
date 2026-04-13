#pragma once
#include "control.h"

namespace augr {

class BarGraphBase : public ParameterControl {
public:
    BarGraphBase(std::string label, Parameter* param, fy_real min, fy_real max, ParameterMeta meta = {})
        : ParameterControl(label, param), min_(min), max_(max), is_db_(false) {}

    // Data members
    fy_real min_, max_;
    bool is_db_; // set by FaustDspUi when zone metadata declares "unit"="dB"

    REFLECT_ENABLE(ParameterControl)
};

} // namespace augr