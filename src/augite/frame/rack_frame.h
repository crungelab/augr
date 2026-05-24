#pragma once

#include "subrack_frame.h"

namespace augr {

class RackFrame : public SubrackFrame {
public:
    RackFrame(RackDoc &doc, Rack &rack, const std::string &label = "");
    ~RackFrame() override;

    void Begin() override;

    // Accessors
    Rack &rack() { return doc().rack(); }
    // Data members

private:
    void RebuildView() override;
};

} // namespace augr