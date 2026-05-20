#pragma once

#include <portable-file-dialogs.h>

#include <augr/rack/rack_doc.h>

#include "../view/subrack_view.h"

#include "../controller/subrack_controller.h"

#include "frame.h"

namespace augr {

class GraphFrame : public FrameT<RackDoc, SubrackView, SubrackController> {
public:
    explicit GraphFrame(const std::string &label = "");
    ~GraphFrame() override;

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
    const RackDoc &doc() const { return FrameT::doc(); }
    SubrackView &view() { return FrameT::view(); }
    SubrackController &controller() { return FrameT::controller(); }

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