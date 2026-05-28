#include <augr/core/archiver_manufacturer.h>
#include <augr/core/archive.h>
#include <augr/core/archiver.h>

#include <augr/rack/rack_doc.h>

#include "rack_viewer.h"

namespace augr {


RackViewer::RackViewer(RackDoc &doc, Rack &rack, const std::string &label)
    : SubrackViewer(doc, rack, label) {
}

RackViewer::~RackViewer() {
}

void RackViewer::OnLoaded() {
    subrack_ = &document().rack();
    SubrackViewer::OnLoaded();
}

void RackViewer::Begin() {
    ImGuiWindowFlags graph_flags =
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::Begin(label_.c_str(), nullptr, graph_flags);
}

} // namespace augr