#pragma once

#include <augr/fm/dexie.h>

#include <augite/view/console_view.h>

namespace augr {

class DexieView : public ViewT<fm::Dexie, ConsoleView> {
public:
    using ViewT<fm::Dexie, ConsoleView>::ViewT;
    void Draw() override;
};

} // namespace augr