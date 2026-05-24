#pragma once

//#include <portable-file-dialogs.h>

#include <augr/rack/rack_doc.h>

#include "../view/subrack_view.h"

#include "../controller/subrack_controller.h"

#include "subrack_frame.h"

namespace augr {

class RackFrame : public SubrackFrame {
public:
    RackFrame(RackDoc &doc, Rack &rack, const std::string &label = "");
    ~RackFrame() override;

    void Begin() override;

    // Accessors
    Rack &rack() { return doc().rack(); }
    // Data members

private:
    void RebuildView() override;

    // View serialization (called from doc hooks).
    nlohmann::json ViewToJson();
    void ViewFromJson(const nlohmann::json &j);

    RackDoc::HookToken save_view_token_ = 0;
    RackDoc::HookToken load_view_token_ = 0;
};

} // namespace augr