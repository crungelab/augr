#pragma once

#include "../widget/widget.h"

namespace augr {

class Inspector : public Widget {
public:
    Inspector() = default;
    virtual ~Inspector() = default;

    void Draw() override;
};
} // namespace augr