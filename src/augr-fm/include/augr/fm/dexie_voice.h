// augr/fm/dexie_voice.h
#pragma once

#include <augr/fm/dx7_patch.h>
#include <augr/rack/voice/voice.h>

#include <vector>

namespace augr {
class Wire;
}

namespace augr::fm {

class Dexie;

class DexieVoice : public Voice {
public:
    void OnCreate() override;
    void LoadPatch(const Dx7Patch& patch);

    REFLECT_ENABLE(Voice)

private:
    Dexie* ops_[6] = {};
    std::vector<Wire*> algorithm_wires_;

    void WireAlgorithm(int algorithm, int feedback_op);
    void PushOperatorParams(int op_idx, const Dx7Op& op, int feedback, int feedback_op);
};

} // namespace augr::fm