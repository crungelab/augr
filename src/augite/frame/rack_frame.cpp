#include <augr/core/archiver_manufacturer.h>
#include <augr/core/archive.h>
#include <augr/core/archiver.h>

#include <augr/rack/rack_doc.h>

#include "rack_frame.h"

namespace augr {


RackFrame::RackFrame(RackDoc &doc, Rack &rack, const std::string &label)
    : SubrackFrame(doc, rack, label) {
}

RackFrame::~RackFrame() {
}

void RackFrame::OnLoaded() {
    subrack_ = &document().rack();
    SubrackFrame::OnLoaded();
}

void RackFrame::Begin() {
    ImGuiWindowFlags graph_flags =
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::Begin(label_.c_str(), nullptr, graph_flags);
}

} // namespace augr