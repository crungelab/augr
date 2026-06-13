#pragma once

#include <string>

#include <portable-file-dialogs.h>
#include <sigslot/signal.hpp>

#include <augr/rack/rack_doc.h>
#include <augr/rack/subrack.h>

#include "../controller/subrack_controller.h"
#include "../view/subrack_view.h"

#include "document_viewer.h"

namespace augr {

// SubrackViewer is a lightweight nested graph-editor frame. Unlike RackViewer
// it does not own a document, manage files, or drive lifecycle — it borrows
// a reference to the project's RackDoc (for MarkModified and the like) and
// displays one Subrack within it.
//
// Created when the user drills into a Subrack node in a parent frame.
// Parented in the widget tree to whichever Frame initiated the drill-in.
class SubrackViewer
    : public DocumentViewerT<RackDoc, Subrack, SubrackView, SubrackController> {
public:
    // doc: the project document (shared with the root RackViewer).
    // subrack: the specific Subrack this frame displays.
    SubrackViewer(const std::string &label, RackDoc &doc, Subrack &subrack);
    ~SubrackViewer();

    void Create() override;
    void OnDestroy() override;

    virtual void OnLoaded();

    virtual void RebuildView();

    void Draw() override;

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

    // Data members
    std::unique_ptr<pfd::open_file> open_dialog_;
    std::unique_ptr<pfd::save_file> save_dialog_;

    PendingAction pending_ = PendingAction::None;
    bool show_unsaved_modal_ = false;

protected:
    // View serialization (called from doc hooks).
    nlohmann::json ViewToJson();
    void ViewFromJson(const nlohmann::json &j);

    sigslot::scoped_connection on_doc_save_conn_;
    sigslot::scoped_connection on_doc_load_conn_;
};

} // namespace augr