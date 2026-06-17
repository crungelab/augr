#pragma once

#include "module_widget.h"

namespace augr {

class Subrack;
class SubrackViewer;

class SubrackWidget : public ModuleWidgetT<Subrack> {
public:
    SubrackWidget(Subrack &model) : ModuleWidgetT<Subrack>(model) {}
    void DrawNodeContent() override;
    // Event handlers
    // Data members
    SubrackViewer *viewer_ = nullptr;
};

} // namespace augr