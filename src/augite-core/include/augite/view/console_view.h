#pragma once

#include "module_view.h"

namespace augr {

class ConsoleView : public ModuleView {
public:
    using ModuleView::ModuleView;
    void Build() override;
};

} // namespace augr