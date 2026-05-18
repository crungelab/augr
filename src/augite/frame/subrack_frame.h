#pragma once

#include <string>

#include <augr/rack/rack_doc.h>
#include <augr/rack/subrack.h>

#include "../view/subrack_view.h"
#include "../controller/subrack_controller.h"

#include "frame.h"

namespace augr {

// SubrackFrame is a lightweight nested graph-editor frame. Unlike RackFrame
// it does not own a document, manage files, or drive lifecycle — it borrows
// a reference to the project's RackDoc (for MarkModified and the like) and
// displays one Subrack within it.
//
// Created when the user drills into a Subrack node in a parent frame.
// Parented in the widget tree to whichever Frame initiated the drill-in.
class SubrackFrame : public FrameT<RackDoc, SubrackView, SubrackController> {
public:
    // doc: the project document (shared with the root RackFrame).
    // subrack: the specific Subrack this frame displays.
    SubrackFrame(RackDoc &doc, Subrack &subrack,
                 const std::string &label = "");
    ~SubrackFrame();

    void RebuildView();

    void Draw() override;
    void Begin() override;

    // Accessors
    Subrack &subrack() { return *subrack_; }
    const Subrack &subrack() const { return *subrack_; }

    RackDoc &doc() { return FrameT::doc(); }
    SubrackView &view() { return FrameT::view(); }
    SubrackController &controller() { return FrameT::controller(); }

    // Data members
    Subrack *subrack_;
};

} // namespace augr