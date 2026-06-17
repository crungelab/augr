#pragma once

#include <vector>

#include "module_widget.h"

namespace augr {

class SpectralModule;

class SpectralWidget : public ModuleWidgetT<SpectralModule> {
public:
    explicit SpectralWidget(SpectralModule& model) : ModuleWidgetT<SpectralModule>(model) {}
};

} // namespace augr