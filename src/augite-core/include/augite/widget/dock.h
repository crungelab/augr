#pragma once

#include <string>

#include <augite/widget/widget.h>

namespace augr {

class Dock : public Widget {
public:
    Dock(const std::string &label = "") : label_(label) {}

    void Draw() override;
    virtual void Begin();
    virtual void End();
    // Data members
    std::string label_;
};

} // namespace augr