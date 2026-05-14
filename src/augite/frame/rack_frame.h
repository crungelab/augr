#pragma once

#include <portable-file-dialogs.h>

#include <augr/rack/rack_doc.h>

#include "../view/rack_view.h"

#include "frame.h"

namespace augr {

class RackFrame : public FrameT<RackDoc, RackView> {
public:
    RackFrame(const std::string &label = "");
    void Draw() override;
    void Begin() override;
    void RebuildView();

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

    // Data members
    std::unique_ptr<pfd::open_file> open_dialog_;
    std::unique_ptr<pfd::save_file> save_dialog_;

    PendingAction pending_ = PendingAction::None;
    bool show_unsaved_modal_ = false;
};

} // namespace augr