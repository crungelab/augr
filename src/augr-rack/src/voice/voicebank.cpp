#include <augr/rack/voice/voicebank.h>

#include <algorithm>

#include <augr/core/model_factory.h>

#include <augr/core/archiver.h>
#include <augr/core/archiver_factory.h>
#include <augr/core/archiver_manufacturer.h>

#include <augr/core/midi/midi_message.h>

#include <augr/rack/module/audio_io.h>
#include <augr/rack/module/midi_io.h>
#include <augr/rack/voice/voice.h>

namespace augr {

void Voicebank::Create(Model *parent) {
    Subrack::Create(parent);
    label_ = "Voicebank";

    // Outer-facing IoModules. The user can wire post-voice FX
    // between voice replicas (summed) and audio_out_module_.
    midi_in_module_ = new MidiInputModule();
    midi_in_module_->Create(this);

    audio_out_module_ = new AudioOutputModule();
    audio_out_module_->Create(this);

    // No master yet -- the user picks one via the inspector dropdown,
    // which calls SetMaster(). Until then, replicas_ stays empty and
    // Process() runs the subrack but produces no voice audio.
}

void Voicebank::SetMaster(Voice *master) {
    EnqueueAction([this, master]() {
        master_ = master;
        master_name_ = master ? master->label_ : std::string();
        // TODO: use a stable id rather than label_ once decided.

        RebuildReplicas(target_voice_count_);
    });
}

void Voicebank::SetVoiceCount(int n) {
    if (n < 1) n = 1;
    if (n == target_voice_count_) return;

    EnqueueAction([this, n]() {
        target_voice_count_ = n;
        if (master_) {
            RebuildReplicas(n);
        }
    });
}

void Voicebank::RebuildReplicas(int n) {
    replicas_.clear();

    if (!master_) return;

    replicas_.reserve(n);

    // Serialize master once, reuse the json for each replica.
    nlohmann::json master_json;
    Archive master_archive(master_json);
    ArchiverManufacturer::singleton().Serialize(master_archive, *master_);

    for (int i = 0; i < n; ++i) {
        auto v = std::make_unique<Voice>();
        v->Create();  // detached (no parent)
        ArchiverManufacturer::singleton().Deserialize(master_archive, *v.get());
        replicas_.push_back(std::move(v));
    }
}

void Voicebank::Process() {
    // Drain MIDI input and dispatch -- even if no master is set, we
    // drain to avoid backlog when a master gets attached later.
    for (const MidiMessage &msg : midi_in_module_->midi_in_->Drain()) {
        HandleMidi(msg);
    }

    // Process the subrack (IoModules and any user-wired post-voice
    // FX).
    Subrack::Process();

    // Process replicas and sum their outputs. If there's no master,
    // replicas_ is empty and both loops are no-ops.
    for (auto &v : replicas_) {
        v->Process();
    }
    SumReplicasIntoOutput();
}

void Voicebank::HandleMidi(const MidiMessage &msg) {
    if (replicas_.empty()) return;

    if (msg.IsNoteOn()) {
        if (msg.velocity() == 0) {
            HandleNoteOff(msg);
        } else {
            HandleNoteOn(msg);
        }
    } else if (msg.IsNoteOff()) {
        HandleNoteOff(msg);
    } else {
        BroadcastToAllVoices(msg);
    }
}

void Voicebank::HandleNoteOn(const MidiMessage &msg) {
    int idx = AllocateVoiceForNote(msg.key());
    if (idx < 0) return;

    Voice *v = replicas_[idx].get();
    // TODO: voicebank-side bookkeeping for stealing:
    // v->SetCurrentNote(msg.key());
    // v->SetNoteOnTick(next_tick_++);
    v->midi_in_->Write(msg);
}

void Voicebank::HandleNoteOff(const MidiMessage &msg) {
    for (auto &v : replicas_) {
        if (v->CurrentNote() == msg.key()) {
            v->midi_in_->Write(msg);
        }
    }
}

void Voicebank::BroadcastToAllVoices(const MidiMessage &msg) {
    for (auto &v : replicas_) {
        v->midi_in_->Write(msg);
    }
}

int Voicebank::AllocateVoiceForNote(int note) {
    if (replicas_.empty()) return -1;

    // 1. Same-note retrigger.
    for (size_t i = 0; i < replicas_.size(); ++i) {
        if (replicas_[i]->CurrentNote() == note && replicas_[i]->IsActive()) {
            return static_cast<int>(i);
        }
    }

    // 2. A free voice.
    for (size_t i = 0; i < replicas_.size(); ++i) {
        if (!replicas_[i]->IsActive()) {
            return static_cast<int>(i);
        }
    }

    // 3. Steal oldest active.
    int oldest = 0;
    uint64_t oldest_tick = replicas_[0]->NoteOnTick();
    for (size_t i = 1; i < replicas_.size(); ++i) {
        if (replicas_[i]->NoteOnTick() < oldest_tick) {
            oldest_tick = replicas_[i]->NoteOnTick();
            oldest = static_cast<int>(i);
        }
    }
    return oldest;
}

void Voicebank::SumReplicasIntoOutput() {
    if (replicas_.empty()) {
        // TODO: write silence to audio_out_module_->audio_out_, or
        // rely on the audio pin defaulting to zero when nothing is
        // written this block.
        return;
    }

    auto mixed = replicas_[0]->audio_out_->Read();
    for (size_t i = 1; i < replicas_.size(); ++i) {
        mixed += replicas_[i]->audio_out_->Read();
    }
    audio_out_module_->audio_out_->Write(mixed);
}

void Voicebank::OnMasterParameterChanged(/* path, value */) {
    // Deferred.
}

} // namespace augr

using namespace augr;
DEFINE_MODEL_FACTORY(Voicebank, "Voicebank", "Polyphony")