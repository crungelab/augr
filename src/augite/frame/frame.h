#pragma once

#include "../widget/widget.h"

namespace augr {

class Frame : public Widget {
public:
    Frame(const std::string &label = "") : Widget(), label_(label) {}
    virtual ~Frame() = default;

    void Draw() override;
    virtual void Begin();
    virtual void End();

    // Accessors
    bool is_active();
    // Data members
    std::string label_;
};

} // namespace augr