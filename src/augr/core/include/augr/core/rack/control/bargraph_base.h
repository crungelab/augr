#pragma once
#include "control.h"

namespace augr {

class BarGraphBase : public Control {
public:
    BarGraphBase(const char *label, fy_real *zone, fy_real min, fy_real max)
        : Control(label, zone), min_(min), max_(max), is_db_(false) {}

    // Data members
    fy_real min_, max_;
    bool is_db_; // set by FaustDspUi when zone metadata declares "unit"="dB"

    REFLECT_ENABLE(Control)
};

} // namespace augr