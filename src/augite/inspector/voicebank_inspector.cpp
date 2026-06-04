#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h> // Required for std::string overloads

#include <augr/rack/voice/voice.h>
#include <augr/rack/voice/voicebank.h>
#include <augr/rack/voice/voice_manager.h>

#include "voicebank_inspector.h"

namespace augr {

void VoicebankInspector::Draw() {
    ModuleInspector::Draw();

    auto &vb = model();
    // Master selection
    {
        //auto master_name = vb.master_name();
        auto master_name = vb.master() ? vb.master()->label() : std::string();
        auto voice_names = VoiceManager::singleton().Names();
        if (ImGui::BeginCombo("Master Voice", master_name.empty() ? "None" : master_name.c_str())) {
            if (ImGui::Selectable("None", master_name.empty())) {
                vb.SetMaster(nullptr);
            }
            for (const auto &name : voice_names) {
                if (ImGui::Selectable(name.c_str(), name == master_name)) {
                    auto *v = VoiceManager::singleton().GetVoice(name);
                    vb.SetMaster(v);
                }
            }
            ImGui::EndCombo();
        }
    }

}

DEFINE_MODEL_WIDGET_FACTORY(VoicebankInspector, Voicebank)

} // namespace augr