#pragma once

#include "subrack_viewer.h"

namespace augr {

class RackViewer : public SubrackViewer {
public:
    RackViewer(const std::string &label, RackDoc &doc, Rack &rack);
    ~RackViewer() override;

    void DrawControls() override;

    void OnLoaded() override;

    void OnDrawMainMenuBar() override;
    void Begin() override;

    // Accessors
    Rack &rack() { return document().model(); }
    // Data members
};

} // namespace augr