#pragma once

#include "module_inspector.h"

namespace augr {

class VoicebankInspector : public ModelWidgetT<Voicebank, ModuleInspector> {
public:
    VoicebankInspector(Voicebank &model) : ModelWidgetT<Voicebank, ModuleInspector>(model) {}
    void Draw() override;
};

} // namespace augr