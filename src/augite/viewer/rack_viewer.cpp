#include <augr/core/archiver_manufacturer.h>
#include <augr/core/archive.h>
#include <augr/core/archiver.h>

#include <augr/rack/rack_doc.h>

#include "rack_viewer.h"

namespace augr {


RackViewer::RackViewer(const std::string &label, RackDoc &doc, Rack &rack)
    : SubrackViewer(label, doc, rack) {
    docked_ = true;
}

RackViewer::~RackViewer() {
}

void RackViewer::OnLoaded() {
    set_model(document().model());
    SubrackViewer::OnLoaded();
}

void RackViewer::Begin() {
    ImGuiWindowFlags graph_flags =
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::Begin(label_.c_str(), nullptr, graph_flags);
}

} // namespace augr