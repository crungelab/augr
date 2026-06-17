#pragma once

#include <augr/fm/dexie.h>

#include <augite/view/module_view.h>

namespace augr {

class DexieView : public ModelViewT<fm::Dexie, ModuleView> {
public:
    using ModelViewT<fm::Dexie, ModuleView>::ModelViewT;
    void Draw() override;
};

} // namespace augr