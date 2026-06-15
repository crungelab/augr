#include <augr/core/model_factory.h>

#include <augr/fm/dexie_voice.h>

#include <augr/fm/dexie.h>
#include <augr/fm/dx7_algorithm.h>
#include <augr/rack/wire.h>

namespace augr::fm {

void DexieVoice::OnCreate() {
    Voice::OnCreate();

    for (int i = 0; i < 6; ++i) {
        auto op = Module::Make<Dexie>(shared_from_this());
        ops_[i] = op.get();
    }
}

void DexieVoice::LoadPatch(const Dx7Patch& patch) {
    const Dx7AlgorithmDef& def = GetDx7Algorithm(patch.algorithm);
    WireAlgorithm(patch.algorithm, def.feedback_op);

    for (int i = 0; i < 6; ++i)
        PushOperatorParams(i, patch.ops[i], patch.feedback, def.feedback_op);
}

void DexieVoice::WireAlgorithm(int algorithm, int feedback_op) {
    for (Wire* w : algorithm_wires_)
        Disconnect(*w);
    algorithm_wires_.clear();

    const Dx7AlgorithmDef& def = GetDx7Algorithm(algorithm);

    for (int r = 0; r < def.route_count; ++r) {
        const auto& route = def.routes[r];
        Connect(*ops_[route.src]->audio_out_, *ops_[route.dst]->cv_phase_in_);
        algorithm_wires_.push_back(wires_.back());
    }

    for (int i = 0; i < 6; ++i) {
        if (def.is_carrier[i]) {
            Connect(*ops_[i]->audio_out_, *audio_out_);
            algorithm_wires_.push_back(wires_.back());
        }
    }
}

void DexieVoice::PushOperatorParams(int op_idx, const Dx7Op& op, int feedback, int feedback_op) {
    Dexie* d = ops_[op_idx];

    for (int i = 0; i < 4; ++i) {
        d->rates_[i]  = op.rates[i];
        d->levels_[i] = op.levels[i];
    }

    d->ratio_coarse_ = op.ratio_coarse;
    d->ratio_fine_   = op.ratio_fine;
    d->detune_       = op.detune;
    d->output_level_ = op.output_level / 99.f;
    d->feedback_     = (op_idx == feedback_op)
        ? static_cast<float>(feedback) / 7.f
        : 0.f;
}

} // namespace augr::fm

using namespace augr;
using namespace augr::fm;

DEFINE_MODEL_FACTORY(DexieVoice, "DexieVoice", "General")