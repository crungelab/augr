#pragma once

#include "module_widget.h"

namespace augr {

class Subrack;

class SubrackWidget : public ModuleWidgetT<Subrack> {
public:
    SubrackWidget(Subrack &model) : ModuleWidgetT<Subrack>(model) {}
    void DrawNodeContent() override;
};

} // namespace augr