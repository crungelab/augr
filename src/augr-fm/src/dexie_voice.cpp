#include <augr/core/archiver_factory.h>
#include <augr/core/model_factory.h>

#include <augr/rack/module/audio_io.h>
#include <augr/rack/module/cv_io.h>
#include <augr/rack/module/midi_io.h>

#include <augr/volt/midi_cv_module.h>

#include <augr/fm/dexie_voice.h>

#include <augr/fm/dexie.h>
#include <augr/fm/dx7_algorithm.h>
#include <augr/rack/wire.h>

#include <augr/rack/archiver/voice_archiver.h>

namespace augr::fm {

void DexieVoice::OnCreate() {
    Voice::OnCreate();
    label_ = "DexieVoice";
}

void DexieVoice::OnCreateFresh() {
    Voice::OnCreateFresh();

    midi_cv_module_ = Model::Make<MidiCvModule>(shared_from_this()).get();
    Connect(*midi_in_module_->midi_out_, *midi_cv_module_->midi_in_);

    for (int i = 0; i < 6; ++i) {
        auto op = Model::Make<Dexie>(shared_from_this());
        op->label_ = "OP" + std::to_string(i + 1);
        Connect(*midi_cv_module_->pitch_out_, *op->cv_pitch_in_);
        Connect(*midi_cv_module_->gate_out_, *op->gate_in_);
        ops_[i] = op.get();
    }
}

void DexieVoice::OnAddingChild(Model &model) {
    Voice::OnAddingChild(model);

    if (auto *midi_cv = dynamic_cast<MidiCvModule *>(&model)) {
        midi_cv_module_ = midi_cv;
    } else if (auto *op = dynamic_cast<Dexie *>(&model)) {
        for (int i = 0; i < 6; ++i) {
            if (!ops_[i]) {
                ops_[i] = op;
                return;
            }
        }
    }
}

void DexieVoice::OnRemovingChild(Model &model) {
    if (auto *midi_cv = dynamic_cast<MidiCvModule *>(&model)) {
        midi_cv_module_ = nullptr;
    } else if (auto *op = dynamic_cast<Dexie *>(&model)) {
        for (int i = 0; i < 6; ++i) {
            if (ops_[i] == op) {
                ops_[i] = nullptr;
                return;
            }
        }
    }

    Voice::OnRemovingChild(model);
}

void DexieVoice::LoadPatch(const Dx7Patch &patch) {
    const Dx7AlgorithmDef &def = GetDx7Algorithm(patch.algorithm);

    WireAlgorithm(patch.algorithm, def.feedback_op);

    for (int i = 0; i < 6; ++i)
        PushOperatorParams(i, patch.ops[i], patch.feedback, def.feedback_op);
}

void DexieVoice::WireAlgorithm(int algorithm, int feedback_op) {
    for (Wire *w : algorithm_wires_)
        Disconnect(*w);
    algorithm_wires_.clear();

    const Dx7AlgorithmDef &def = GetDx7Algorithm(algorithm);

    for (int i = 0; i < 6; ++i)
        is_carrier_[i] = def.is_carrier[i];

    // Wire modulators to their targets.
    for (int r = 0; r < def.route_count; ++r) {
        const auto &route = def.routes[r];
        Connect(*ops_[route.src]->audio_out_, *ops_[route.dst]->cv_phase_in_);
        algorithm_wires_.push_back(wires_.back());
    }

    // Wire carriers to the AudioOutputModule's input — this keeps
    // all wires within the child graph and avoids boundary crossing.
    for (int i = 0; i < 6; ++i) {
        if (is_carrier_[i]) {
            Connect(*ops_[i]->audio_out_, *audio_out_module_->audio_in_);
            algorithm_wires_.push_back(wires_.back());
        }
    }
}

// DX7 output level is roughly logarithmic. This is an approximation —
// calibrate the dB-per-step against Dexed by ear.
float OutputLevelToGain(float level_0_99) {
    if (level_0_99 <= 0.f) return 0.f;
    constexpr float kDbPerStep = 0.75f;          // tune this
    const float db = (level_0_99 - 99.f) * kDbPerStep;
    return std::pow(10.f, db / 20.f);
}

void DexieVoice::PushOperatorParams(int op_idx, const Dx7Op &op, int feedback,
                                    int feedback_op) {
    Dexie *d = ops_[op_idx];

    for (int i = 0; i < 4; ++i) {
        d->rates_[i] = op.rates[i];
        d->levels_[i] = op.levels[i];
    }

    d->ratio_coarse_ = op.ratio_coarse;
    d->ratio_fine_ = op.ratio_fine;
    d->detune_ = op.detune;
    //d->output_level_ = op.output_level / 99.f;
    d->output_level_ = OutputLevelToGain(op.output_level);
    //d->feedback_ = 0.0f;
    d->feedback_ =
        (op_idx == feedback_op) ? static_cast<float>(feedback) / 7.f : 0.f;
}

} // namespace augr::fm
using namespace augr;
using namespace augr::fm;

DEFINE_MODEL_FACTORY(DexieVoice, "DexieVoice", "Fm")

class DexieVoiceArchiver : public VoiceArchiver {};
DEFINE_ARCHIVER_FACTORY(DexieVoiceArchiver, DexieVoice, "DexieVoice")
