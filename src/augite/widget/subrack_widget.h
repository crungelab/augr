#pragma once

#include "module_widget.h"

namespace augr {

class Subrack;
class SubrackViewer;

class SubrackWidget : public ModuleWidgetT<Subrack> {
public:
    SubrackWidget(Subrack &model) : ModuleWidgetT<Subrack>(model) {}
    void DrawNodeContent() override;
    void DrawViewer() override {}
    // Event handlers
    void OnLeftDoubleClick(RackDoc &doc, Frame &parent_frame) override;
    // Data members
    SubrackViewer *viewer_ = nullptr;
};

} // namespace augr