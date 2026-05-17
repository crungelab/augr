#pragma once

#include <portable-file-dialogs.h>

#include <augr/rack/rack_doc.h>

#include "../view/rack_view.h"

#include "frame.h"

namespace augr {

class RackFrame : public FrameT<RackDoc, RackView> {
public:
    RackFrame(const std::string &label = "");
    ~RackFrame();

    void Draw() override;
    void Begin() override;

    void PollPendingDialog();
    void DrawMenuBar();
    void StartOpenDialog();
    void StartSaveAsDialog();

    void DoNew();
    void DoOpen(const std::filesystem::path &p);
    void DoSave();
    void DoSaveAs(const std::filesystem::path &p);
    void DrawUnsavedModal();

    enum class PendingAction {
        None,
        Open,
        SaveAs,
        NewAfterPrompt,
        OpenAfterPrompt
    };

    // Accessors
    Rack &rack() { return doc().rack(); }
    RackDoc &doc() { return FrameT::doc(); }
    RackView &view() { return FrameT::view(); }

    // Data members
    std::unique_ptr<pfd::open_file> open_dialog_;
    std::unique_ptr<pfd::save_file> save_dialog_;

    PendingAction pending_ = PendingAction::None;
    bool show_unsaved_modal_ = false;

private:
    void RebuildView();

    // View serialization (called from doc hooks).
    nlohmann::json ViewToJson();
    void ViewFromJson(const nlohmann::json &j);

    // ... existing dialog/menu methods ...

    RackDoc::HookToken save_view_token_ = 0;
    RackDoc::HookToken load_view_token_ = 0;
};

} // namespace augr