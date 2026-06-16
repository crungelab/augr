// augr/fm/dexie_voice.h
#pragma once

#include <augr/fm/dx7_patch.h>
#include <augr/rack/voice/voice.h>

#include <vector>

namespace augr {
class Wire;
class MidiCvModule;
}

namespace augr::fm {

class Dexie;

class DexieVoice : public Voice {
public:
    friend class DexieVoiceArchiver;

    void OnCreate() override;
    void OnCreateFresh() override;
    void OnAddingChild(Model &model) override;
    void OnRemovingChild(Model &model) override;
    void LoadPatch(const Dx7Patch& patch);

    REFLECT_ENABLE(Voice)

private:
    MidiCvModule* midi_cv_module_ = nullptr;
    Dexie* ops_[6]      = {};
    bool is_carrier_[6] = {};
    std::vector<Wire*> algorithm_wires_;

    void WireAlgorithm(int algorithm, int feedback_op);
    void PushOperatorParams(int op_idx, const Dx7Op& op,
                            int feedback, int feedback_op);
};

} // namespace augr::fm