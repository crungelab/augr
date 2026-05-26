#pragma once

#include "subrack_frame.h"

namespace augr {

class RackFrame : public SubrackFrame {
public:
    RackFrame(RackDoc &doc, Rack &rack, const std::string &label = "");
    ~RackFrame() override;

    void OnLoaded() override;

    void Begin() override;

    // Accessors
    Rack &rack() { return document().rack(); }
    // Data members
};

} // namespace augr