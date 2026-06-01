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

void Voicebank::Create() {
    Subrack::Create();
    label_ = "Voicebank";

    // No master yet -- the user picks one via the inspector dropdown,
    // which calls SetMaster(). Until then, replicas_ stays empty and
    // Process() runs the subrack but produces no voice audio.
}

void Voicebank::OnFresh() {
    // Outer-facing IoModules. The user can wire post-voice FX
    // between voice replicas (summed) and audio_out_module_.
    midi_in_module_ = &Model::Make<MidiInputModule>(this);

    audio_out_module_ = &Model::Make<AudioOutputModule>(this);
}

void Voicebank::OnAddingChild(Model &model) {
    Subrack::OnAddingChild(model);

    if (auto *d = dynamic_cast<Io *>(&model)) {
        OnAddingIo(*d);
    } else if (auto *v = dynamic_cast<Voice *>(&model)) {
        OnAddingVoice(*v);
    }
}

void Voicebank::OnRemovingChild(Model &model) {
    if (auto *d = dynamic_cast<Io *>(&model)) {
        OnRemovingIo(*d);
    } else if (auto *v = dynamic_cast<Voice *>(&model)) {
        OnRemovingVoice(*v);
    }
    Subrack::OnRemovingChild(model);
}

void Voicebank::OnAddingIo(Io &io) {
    if (auto *audio_output = dynamic_cast<AudioOutputModule *>(&io)) {
        audio_out_module_ = audio_output;
        audio_out_ = audio_out_module_->audio_out_;
    } else if (auto *midi_input = dynamic_cast<MidiInputModule *>(&io)) {
        midi_in_module_ = midi_input;
        midi_in_ = midi_in_module_->midi_in_;
    }
}

void Voicebank::OnRemovingIo(Io &io) {
    if (audio_out_module_ == &io)
        audio_out_module_ = nullptr;
    if (midi_in_module_ == &io)
        midi_in_module_ = nullptr;
}

void Voicebank::OnAddingVoice(Voice &voice) {
    replicas_.push_back(std::unique_ptr<Voice>(&voice));
}

void Voicebank::OnRemovingVoice(Voice &voice) {
    replicas_.erase(std::remove_if(replicas_.begin(), replicas_.end(),
                                   [&voice](const std::unique_ptr<Voice> &v) {
                                       return v.get() == &voice;
                                   }),
                    replicas_.end());
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
    if (n < 1)
        n = 1;
    if (n == target_voice_count_)
        return;

    EnqueueAction([this, n]() {
        target_voice_count_ = n;
        if (master_) {
            RebuildReplicas(n);
        }
    });
}

void Voicebank::RebuildReplicas(int n) {
    replicas_.clear();

    if (!master_)
        return;

    replicas_.reserve(n);

    // Serialize master once, reuse the json for each replica.
    nlohmann::json master_json;
    Archive master_archive(master_json);
    ArchiverManufacturer::singleton().Serialize(master_archive, *master_);

    for (int i = 0; i < n; ++i) {
        Voice *v = &Model::Make<Voice>(this);
        ArchiverManufacturer::singleton().Deserialize(master_archive, *v);
        // replicas_.push_back(std::unique_ptr<Voice>(v));
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
    /*
    for (auto &v : replicas_) {
        v->Process();
    }
    */
    SumReplicasIntoOutput();
}

void Voicebank::HandleMidi(const MidiMessage &msg) {
    if (replicas_.empty())
        return;

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
    if (idx < 0)
        return;

    Voice *v = replicas_[idx].get();
    // TODO: voicebank-side bookkeeping for stealing:
    v->set_current_note(msg.key());
    v->set_note_on_tick(next_tick_++);
    v->midi_in_module_->midi_out_->Write(msg);
}

/*
void Voicebank::HandleNoteOff(const MidiMessage &msg) {
    for (auto &v : replicas_) {
        if (v->current_note() == msg.key()) {
            v->midi_in_module_->midi_out_->Write(msg);
        }
    }
}
*/
void Voicebank::HandleNoteOff(const MidiMessage &msg) {
    for (auto &v : replicas_) {
        if (v->current_note() == msg.key()) {
            v->midi_in_module_->midi_out_->Write(msg);
            v->set_current_note(-1); // <-- release the assignment
        }
    }
}

void Voicebank::BroadcastToAllVoices(const MidiMessage &msg) {
    for (auto &v : replicas_) {
        v->midi_in_module_->midi_out_->Write(msg);
    }
}

bool Voicebank::VoiceIsFree(const Voice &v) const {
    return v.current_note() < 0 && !v.IsActive();
}

int Voicebank::AllocateVoiceForNote(int note) {
    if (replicas_.empty())
        return -1;

    // 1. Same-note retrigger.
    for (size_t i = 0; i < replicas_.size(); ++i) {
        if (replicas_[i]->current_note() == note && replicas_[i]->IsActive()) {
            return static_cast<int>(i);
        }
    }

    // 2. A free voice.
    for (size_t i = 0; i < replicas_.size(); ++i) {
        if (VoiceIsFree(*replicas_[i])) {
            return static_cast<int>(i);
        }
    }

    // 3. Steal oldest active.
    int oldest = 0;
    uint64_t oldest_tick = replicas_[0]->note_on_tick();
    for (size_t i = 1; i < replicas_.size(); ++i) {
        if (replicas_[i]->note_on_tick() < oldest_tick) {
            oldest_tick = replicas_[i]->note_on_tick();
            oldest = static_cast<int>(i);
        }
    }
    return oldest;
}

/*
int Voicebank::AllocateVoiceForNote(int note) {
    if (replicas_.empty())
        return -1;

    // 1. Same-note retrigger.
    for (size_t i = 0; i < replicas_.size(); ++i) {
        if (replicas_[i]->current_note() == note && replicas_[i]->IsActive()) {
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
    uint64_t oldest_tick = replicas_[0]->note_on_tick();
    for (size_t i = 1; i < replicas_.size(); ++i) {
        if (replicas_[i]->note_on_tick() < oldest_tick) {
            oldest_tick = replicas_[i]->note_on_tick();
            oldest = static_cast<int>(i);
        }
    }
    return oldest;
}
*/

void Voicebank::SumReplicasIntoOutput() {
    if (replicas_.empty()) {
        audio_out_module_->audio_out_->Write(Audio());
        return;
    }

    auto mixed = replicas_[0]->audio_out_->Read();
    for (size_t i = 1; i < replicas_.size(); ++i) {
        if (!replicas_[i]->IsActive())
            continue; // skip silent voices to reduce noise from summing
        mixed += replicas_[i]->audio_out_->Read();
    }
    audio_out_module_->audio_out_->Write(mixed);
}

/*
void Voicebank::SumReplicasIntoOutput() {
    if (replicas_.empty()) {
        audio_out_module_->audio_out_->Write(Audio());
        return;
    }

    auto mixed = replicas_[0]->audio_out_->Read();
    for (size_t i = 1; i < replicas_.size(); ++i) {
        mixed += replicas_[i]->audio_out_->Read();
    }
    audio_out_module_->audio_out_->Write(mixed);
}
*/

void Voicebank::OnMasterParameterChanged(/* path, value */) {
    // Deferred.
}

} // namespace augr

using namespace augr;
DEFINE_MODEL_FACTORY(Voicebank, "Voicebank", "Polyphony")