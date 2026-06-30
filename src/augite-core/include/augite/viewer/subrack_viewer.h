#pragma once

#include <string>

#include <portable-file-dialogs.h>

#include <augr/rack/rack_doc.h>
#include <augr/rack/subrack.h>

#include "../controller/subrack_controller.h"
#include "../view/subrack_view.h"

#include "viewer.h"

namespace augr {

class ConsoleView;

class SubrackViewer
    : public ViewerT<RackDoc, Subrack, SubrackView, SubrackController> {
public:
    friend class SubrackViewerArchiver;
    // doc: the project document (shared with the root RackViewer).
    // subrack: the specific Subrack this frame displays.
    SubrackViewer(const std::string &label, RackDoc &doc, Subrack &subrack);
    ~SubrackViewer();

    void RebuildView() override;
    void RebuildConsoleView();

    void Draw() override;
    void DrawConsole();
    virtual void DrawControls();

    void PollPendingDialog();
    void OnDrawMainMenuBar() override;

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

    std::unique_ptr<ConsoleView> console_view_;
};

} // namespace augr