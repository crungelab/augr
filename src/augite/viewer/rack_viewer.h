#pragma once

#include "subrack_viewer.h"

namespace augr {

class RackViewer : public SubrackViewer {
public:
    RackViewer(RackDoc &doc, Rack &rack, const std::string &label = "");
    ~RackViewer() override;

    void OnLoaded() override;

    void Begin() override;

    // Accessors
    Rack &rack() { return document().rack(); }
    // Data members
};

} // namespace augr