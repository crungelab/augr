#pragma once

#include <string>

#include <portable-file-dialogs.h>

#include <augr/rack/rack_doc.h>
#include <augr/rack/subrack.h>

#include "../controller/subrack_controller.h"
#include "../view/subrack_view.h"

#include "frame.h"

namespace augr {

// SubrackFrame is a lightweight nested graph-editor frame. Unlike RackFrame
// it does not own a document, manage files, or drive lifecycle — it borrows
// a reference to the project's RackDoc (for MarkModified and the like) and
// displays one Subrack within it.
//
// Created when the user drills into a Subrack node in a parent frame.
// Parented in the widget tree to whichever Frame initiated the drill-in.
class SubrackFrame : public FrameT<RackDoc, SubrackView, SubrackController> {
public:
    // doc: the project document (shared with the root RackFrame).
    // subrack: the specific Subrack this frame displays.
    SubrackFrame(RackDoc &doc, Subrack &subrack, const std::string &label = "");
    ~SubrackFrame();

    void Create(Widget *parent = nullptr) override;

    virtual void OnLoaded();
    
    virtual void RebuildView();

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
    Subrack &subrack() { return *subrack_; }
    const Subrack &subrack() const { return *subrack_; }
   
    // Data members
    Subrack *subrack_;

    std::unique_ptr<pfd::open_file> open_dialog_;
    std::unique_ptr<pfd::save_file> save_dialog_;

    PendingAction pending_ = PendingAction::None;
    bool show_unsaved_modal_ = false;
protected:
    // View serialization (called from doc hooks).
    nlohmann::json ViewToJson();
    void ViewFromJson(const nlohmann::json &j);

    RackDoc::HookToken save_view_token_ = 0;
    RackDoc::HookToken load_view_token_ = 0;

};

} // namespace augr