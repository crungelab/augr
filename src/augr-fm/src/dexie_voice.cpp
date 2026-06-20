#include <augr/archiver_factory.h>
#include <augr/model_factory.h>

#include <augr/rack/module/audio_io.h>
#include <augr/rack/module/cv_io.h>
#include <augr/rack/module/midi_io.h>
#include <augr/rack/wire.h>

#include <augr/rack/archiver/voice_archiver.h>

#include <augr/volt/lfo_module.h>
#include <augr/volt/midi_cv_module.h>

#include <augr/fm/dexie_voice.h>

#include <augr/fm/dexie.h>
#include <augr/fm/dexie_lfo.h>

#include <augr/fm/dexie_common.h>
#include <augr/fm/dexie_pitch_env_module.h>
#include <augr/fm/dx7_algorithm.h>

namespace augr::fm {

LfoModule::Waveform Dx7WaveformToLfoWaveform(int dx7_waveform) {
    switch (dx7_waveform) {
    case 0:
        return LfoModule::Waveform::Tri;
    case 1:
        return LfoModule::Waveform::SawDown;
    case 2:
        return LfoModule::Waveform::SawUp;
    case 3:
        return LfoModule::Waveform::Square;
    case 4:
        return LfoModule::Waveform::Sine;
    case 5:
        return LfoModule::Waveform::SampleHold;
    default:
        return LfoModule::Waveform::Sine;
    }
}

void DexieVoice::OnCreate() {
    Voice::OnCreate();
    label_ = "DexieVoice";
}

void DexieVoice::OnCreateFresh() {
    Voice::OnCreateFresh();

    midi_cv_module_ = Model::Make<MidiCvModule>(shared_from_this()).get();
    Connect(*midi_in_module_->midi_out_, *midi_cv_module_->midi_in_);

    lfo_module_ = Model::Make<LfoModule>(shared_from_this()).get();

    pitch_env_module_ =
        Model::Make<DexiePitchEnvModule>(shared_from_this()).get();
    Connect(*midi_cv_module_->gate_out_, *pitch_env_module_->gate_in_);

    for (int i = 0; i < 6; ++i) {
        auto op = Model::Make<Dexie>(shared_from_this());
        op->label_ = "OP" + std::to_string(i + 1);
        Connect(*midi_cv_module_->pitch_out_, *op->cv_pitch_in_);
        Connect(*midi_cv_module_->gate_out_, *op->gate_in_);
        Connect(*lfo_module_->cv_out_, *op->cv_amp_mod_in_);
        Connect(*lfo_module_->cv_out_, *op->cv_pitch_mod_in_);
        Connect(*midi_cv_module_->velocity_out_, *op->cv_velocity_in_);
        Connect(*pitch_env_module_->cv_out_, *op->cv_pitch_env_in_);
        ops_[i] = op.get();
    }
}

void DexieVoice::OnAddingChild(Model &model) {
    Voice::OnAddingChild(model);

    if (auto *midi_cv = dynamic_cast<MidiCvModule *>(&model)) {
        midi_cv_module_ = midi_cv;
    } else if (auto *m = dynamic_cast<DexiePitchEnvModule *>(&model)) {
        pitch_env_module_ = m;
    } else if (auto *op = dynamic_cast<Dexie *>(&model)) {
        for (auto &i : ops_) {
            if (!i) {
                i = op;
                return;
            }
        }
    }
}

void DexieVoice::OnRemovingChild(Model &model) {
    if (auto *midi_cv = dynamic_cast<MidiCvModule *>(&model)) {
        midi_cv_module_ = nullptr;
    } else if (auto *m = dynamic_cast<DexiePitchEnvModule *>(&model)) {
        pitch_env_module_ = nullptr;
    } else if (auto *op = dynamic_cast<Dexie *>(&model)) {
        for (auto &i : ops_) {
            if (i == op) {
                i = nullptr;
            }
        }
    }

    Voice::OnRemovingChild(model);
}

// dexie_voice.cpp
void DexieVoice::LoadPatch(const Dx7Patch &patch) {
    const Dx7AlgorithmDef &def = GetDx7Algorithm(patch.algorithm);
    WireAlgorithm(patch.algorithm, def.feedback_op);

    for (int i = 0; i < 6; ++i)
        PushOperatorParams(i, patch.ops[i], patch.feedback, def);

    if (lfo_module_) {
        lfo_module_->rate_ = DexieLfo::SpeedToOctaves(patch.lfo_speed);
        lfo_module_->waveform_ = Dx7WaveformToLfoWaveform(patch.lfo_waveform);
    }

    if (pitch_env_module_) {
        int rates[4], levels[4];
        for (int i = 0; i < 4; ++i) {
            rates[i] = patch.pitch_eg_rates[i];
            levels[i] = patch.pitch_eg_levels[i];
        }
        pitch_env_module_->SetRatesLevels(rates, levels);
    }
    // Convert the patch's 0..99 LFO delay to a sample count, and propagate
    // depth/delay to every operator (delay ramp and depth scaling are
    // applied per-operator inside Dexie::Process, alongside each operator's
    // own AMS).
    const float sample_rate = Audio::sample_rate();
    constexpr float kMaxDelaySeconds = 5.0f; // placeholder — needs calibration
    const int delay_samples = static_cast<int>((patch.lfo_delay / 99.0f) *
                                               kMaxDelaySeconds * sample_rate);

    for (int i = 0; i < 6; ++i) {
        ops_[i]->lfo_amp_depth_ = static_cast<float>(patch.lfo_amp_depth);
        ops_[i]->lfo_pitch_depth_ = static_cast<float>(patch.lfo_pitch_depth);
        ops_[i]->pitch_mod_sens_ = static_cast<float>(patch.pitch_mod_sens);
        ops_[i]->lfo_delay_samples_total_ =
            static_cast<int>((patch.lfo_delay / 99.0f) * kMaxDelaySeconds *
                             Audio::sample_rate());
        ops_[i]->osc_key_sync_ = (patch.osc_key_sync != 0);
        ops_[i]->transpose_ = patch.transpose - 24;
    }

    // Disconnect existing LFO sync wire if present
    if (lfo_sync_wire_) {
        Disconnect(*lfo_sync_wire_);
        lfo_sync_wire_ = nullptr;
    }

    // Wire gate to LFO reset if patch requests sync
    if (patch.lfo_sync && lfo_module_) {
        lfo_sync_wire_ =
            Connect(*midi_cv_module_->gate_out_, *lfo_module_->reset_in_);
    }
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
        auto wire = Connect(*ops_[route.src]->audio_out_,
                            *ops_[route.dst]->cv_phase_in_);
        algorithm_wires_.push_back(wire);
    }

    // Wire carriers to the AudioOutputModule's input — this keeps
    // all wires within the child graph and avoids boundary crossing.
    for (int i = 0; i < 6; ++i) {
        if (is_carrier_[i]) {
            auto wire =
                Connect(*ops_[i]->audio_out_, *audio_out_module_->audio_in_);
            algorithm_wires_.push_back(wire);
        }
    }
}

void DexieVoice::PushOperatorParams(int op_idx, const Dx7Op &op, int feedback,
                                    const Dx7AlgorithmDef &def) {
    Dexie *d = ops_[op_idx];

    for (int i = 0; i < 4; ++i) {
        d->rates_[i] = op.rates[i];
        d->levels_[i] = op.levels[i];
    }

    d->ratio_coarse_ = op.ratio_coarse;
    d->ratio_fine_ = op.ratio_fine;
    d->detune_ = op.detune;

    // Output level is the raw DX7 0..99 value now — DexieEnv applies the
    // scaleoutlevel curve and folds it into the envelope's target levels
    // internally, so there is no separate gain conversion here.
    d->output_level_ = op.output_level;

    // feedback_ is the raw DX7 0..7 patch amount. Dexie::Process converts it
    // to a feedback shift. Only the algorithm's designated feedback operator
    // gets a nonzero value; everyone else stays at 0 (off).
    d->feedback_ =
        (op_idx == def.feedback_op) ? static_cast<float>(feedback) : 0.f;

    d->amp_mod_sens_ = static_cast<float>(op.amp_mod_sens);
    d->velocity_sens_ = static_cast<float>(op.velocity_sens);

    d->kbd_break_pt_ = op.kbd_break_pt;
    d->kbd_left_depth_ = op.kbd_left_depth;
    d->kbd_right_depth_ = op.kbd_right_depth;
    d->kbd_left_curve_ = op.kbd_left_curve;
    d->kbd_right_curve_ = op.kbd_right_curve;

    // d->kbd_rate_scaling_ = op.kbd_rate_scaling;
    d->kbd_rate_scaling_ = static_cast<float>(op.kbd_rate_scaling);

    d->fixed_freq_ = op.fixed_freq;
    /*
    d->frequency_ =
        op.fixed_freq ? FixedFrequencyHz(op.coarse_raw, op.fine_raw) : 0.0f;
    */
    d->frequency_ = op.fixed_freq ? FixedFrequencyHz(op.coarse_raw, op.fine_raw,
                                                     op.detune_raw)
                                  : 0.0f;
}

} // namespace augr::fm
using namespace augr;
using namespace augr::fm;

DEFINE_MODEL_FACTORY(DexieVoice, "DexieVoice", "Fm")

class DexieVoiceArchiver : public VoiceArchiver {};
DEFINE_ARCHIVER_FACTORY(DexieVoiceArchiver, DexieVoice, "DexieVoice")