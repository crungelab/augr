#include <augr/archive.h>
#include <augr/archiver.h>
#include <augr/archiver_manufacturer.h>

#include <augite/app/app.h>

#include <augite/viewer/viewer.h>

#include "imgui.h"

namespace augr {

Viewer::Viewer(const std::string &label, Document &doc, Model &model)
    : Frame(label), doc_(&doc), model_(&model) {
    on_doc_save_conn_ = doc_->on_save.connect([this]() { SaveViewerState(); });
    on_doc_load_conn_ = doc_->on_load.connect([this]() { OnLoaded(); });
}

void Viewer::Create() {
    Frame::Create();
    RebuildView();
    RestoreViewerState();
}

void Viewer::OnDestroy() {
    SaveViewerState();
    on_doc_save_conn_.disconnect();
    on_doc_load_conn_.disconnect();
    Frame::OnDestroy();
}

void Viewer::OnLoaded() {
    DestroyChildren();
    RebuildView();
    RestoreViewerState();
}

void Viewer::SaveViewerState() {
    nlohmann::json out;
    Archive archive(out);
    ArchiverManufacturer::singleton().Serialize(archive, *this);
    document().viewers_[model().uuid()] = std::move(out);
}

void Viewer::RestoreViewerState() {
    auto it = document().viewers_.find(model().uuid());
    if (it == document().viewers_.end())
        return;

    Archive archive(it->second);
    ArchiverManufacturer::singleton().Deserialize(archive, *this);
}

void Viewer::Begin() {
    // Use a unique ImGui window ID per viewer so multiple frames
    // (different viewers) don't collide. The viewer pointer is stable
    // for the frame's lifetime.
    char title[128];
    std::snprintf(title, sizeof(title), "%s###viewer_%p",
                  label_.empty() ? "Viewer" : label_.c_str(),
                  static_cast<void *>(&model()));
    bool p_open = true;
    ImGui::Begin(title, &p_open, ImGuiWindowFlags_NoCollapse);
    if (!p_open) {
        App::singleton().EnqueueAction([this]() {
            App::singleton().viewer_manager().CloseViewer(*this);
        });
    }
}

void Viewer::End() {
    if (view_)
        view_->Draw();
    if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)) {
        App::singleton().set_active_frame(this);
        if (controller_)
            controller_->Control();
    }

    Frame::End();
}

} // namespace augr