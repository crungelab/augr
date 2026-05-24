#include <augr/core/archiver_manufacturer.h>
#include <augr/core/archive.h>
#include <augr/core/archiver.h>

#include <augr/rack/rack_doc.h>

#include "rack_frame.h"

namespace augr {


RackFrame::RackFrame(RackDoc &_doc, Rack &rack, const std::string &label)
    : SubrackFrame(_doc, rack, label) {
    // Install view hooks BEFORE NewDocument, so the load-hook
    // gets invoked on the initial document. RackFrame owns both doc_
    // and view_, so the `this` captures are valid for the lifetime of
    // the doc_.
    save_view_token_ = doc_->AddSaveHook("view", [this]() {
        return ViewToJson();
    });
    load_view_token_ = doc_->AddLoadHook("view", [this](const nlohmann::json &j) {
        RebuildView();
        if (!j.empty()) {
            ViewFromJson(j);
        }
    });
}

RackFrame::~RackFrame() {
    if (doc_) {
        doc_->RemoveSaveHook(save_view_token_);
        doc_->RemoveLoadHook(load_view_token_);
    }
}

void RackFrame::RebuildView() {
    view_ = std::make_unique<SubrackView>(doc());
    view().Build();  // construct widget tree now so view archiver has something to load into
    controller_ = std::make_unique<SubrackController>(doc(), view(), *this);
}

nlohmann::json RackFrame::ViewToJson() {
    if (!view_) return nlohmann::json();

    auto &mfr = ArchiverManufacturer::singleton();
    auto *factory = mfr.FindFactory(std::type_index(typeid(*view_)));
    if (!factory) {
        std::cerr << "No archiver factory for SubrackView\n";
        return nlohmann::json();
    }
    std::unique_ptr<Archiver> archiver(factory->Produce(*view_));
    nlohmann::json out;
    Archive archive(out);
    archiver->Save(archive);
    return out;
}

void RackFrame::ViewFromJson(const nlohmann::json &j) {
    if (!view_) return;

    auto &mfr = ArchiverManufacturer::singleton();
    auto *factory = mfr.FindFactory(std::type_index(typeid(*view_)));
    if (!factory) return;

    std::unique_ptr<Archiver> archiver(factory->Produce(*view_));
    nlohmann::json local = j;
    Archive archive(local);
    archiver->Load(archive);
}

void RackFrame::Begin() {
    ImGuiWindowFlags graph_flags =
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::Begin(label_.c_str(), nullptr, graph_flags);
}

} // namespace augr