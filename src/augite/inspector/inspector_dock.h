#pragma once

#include "../widget/widget.h"

namespace augr {

class InspectorDock : public Widget {
public:
    InspectorDock(Widget::Ptr root = nullptr) : root_(std::move(root)) {}
    virtual ~InspectorDock() = default;

    void Draw() override;

    // Data members
    Widget::Ptr root_;
};
} // namespace augr