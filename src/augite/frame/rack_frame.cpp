#include <augr/core/archiver_manufacturer.h>
#include <augr/core/archive.h>
#include <augr/core/archiver.h>

#include <augr/rack/rack_doc.h>

#include "rack_frame.h"

namespace augr {


RackFrame::RackFrame(RackDoc &_doc, Rack &rack, const std::string &label)
    : SubrackFrame(_doc, rack, label) {
}

RackFrame::~RackFrame() {
}

void RackFrame::RebuildView() {
    view_ = std::make_unique<SubrackView>(doc());
    view().Build();  // construct widget tree now so view archiver has something to load into
    controller_ = std::make_unique<SubrackController>(doc(), view(), *this);

    // If we have a cached view state for this subrack, apply it.
    auto it = doc().views_.find(rack().uuid());
    if (it != doc().views_.end()) {
        ViewFromJson(it->second);
    }
}

void RackFrame::Begin() {
    ImGuiWindowFlags graph_flags =
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::Begin(label_.c_str(), nullptr, graph_flags);
}

} // namespace augr