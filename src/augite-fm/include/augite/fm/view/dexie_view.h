#pragma once

#include <augr/fm/dexie.h>

#include <augite/view/console_view.h>

namespace augr {

class DexieView : public ModelViewT<fm::Dexie, ConsoleView> {
public:
    using ModelViewT<fm::Dexie, ConsoleView>::ModelViewT;
    void Draw() override;
};

} // namespace augr