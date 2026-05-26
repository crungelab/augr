#include <augr/core/archiver_manufacturer.h>
#include <augr/core/archive.h>
#include <augr/core/archiver.h>

#include <augr/rack/rack_doc.h>

#include "rack_frame.h"

namespace augr {


RackFrame::RackFrame(RackDoc &doc, Rack &rack, const std::string &label)
    : SubrackFrame(doc, rack, label) {
    load_subrack_token_ = doc_->AddLoadHook([this](const nlohmann::json &) {
        subrack_ = &document().rack();
    });
}

RackFrame::~RackFrame() {
    if (doc_) {
        doc_->RemoveLoadHook(load_subrack_token_);
    }
}

void RackFrame::RebuildView() {
    view_ = std::make_unique<SubrackView>(document());
    view().Build();  // construct widget tree now so view archiver has something to load into
    controller_ = std::make_unique<SubrackController>(document(), view(), *this);

    // If we have a cached view state for this subrack, apply it.
    auto it = document().views_.find(rack().uuid());
    if (it != document().views_.end()) {
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