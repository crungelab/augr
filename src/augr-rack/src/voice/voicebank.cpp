#include <cmath>

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

void Voicebank::OnCreate() {
    Subrack::OnCreate();
    label_ = "Voicebank";
}

void Voicebank::OnCreateFresh() {
    Subrack::OnCreateFresh();

    auto midi_in = Model::Make<MidiInputModule>(shared_from_this());
    midi_in_module_ = midi_in.get();
    midi_in_ = midi_in_module_->midi_in_;

    auto audio_out = Model::Make<AudioOutputModule>(shared_from_this());
    audio_out_module_ = audio_out.get();
    audio_out_ = audio_out_module_->audio_out_;
}

void Voicebank::OnAddingChild(Model &model) {
    Subrack::OnAddingChild(model);
    if (auto *v = dynamic_cast<Voice *>(&model))
        OnAddingVoice(*v);
}

void Voicebank::OnRemovingChild(Model &model) {
    if (auto *v = dynamic_cast<Voice *>(&model))
        OnRemovingVoice(*v);
    Subrack::OnRemovingChild(model);
}

void Voicebank::OnAddingIo(Io &io) {
    Subrack::OnAddingIo(io);
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
    Subrack::OnRemovingIo(io);
}

void Voicebank::OnAddingVoice(Voice &voice) { replicas_.push_back(&voice); }

void Voicebank::OnRemovingVoice(Voice &voice) {
    // If the voice being removed is our master, clear the weak_ptr.
    if (master_.lock().get() == &voice)
        master_.reset();

    replicas_.erase(std::remove(replicas_.begin(), replicas_.end(), &voice),
                    replicas_.end());
}

void Voicebank::SetMaster(Voice *master) {
    std::weak_ptr<Voice> weak;
    if (master) {
        weak = std::dynamic_pointer_cast<Voice>(master->shared_from_this());
        on_master_topology_changed_conn_ = master->on_topology_changed.connect(
            [this]() { RebuildReplicas(target_voice_count_); });
    } else {
        on_master_topology_changed_conn_.disconnect();
    }

    EnqueueAction([this, weak]() {
        master_ = weak;
        RebuildReplicas(target_voice_count_);
    });
}

/*
void Voicebank::SetMaster(Voice *master) {
    std::weak_ptr<Voice> weak;
    if (master)
        weak = std::dynamic_pointer_cast<Voice>(master->shared_from_this());

    EnqueueAction([this, weak]() {
        master_ = weak;
        RebuildReplicas(target_voice_count_);
    });
}
*/

void Voicebank::SetVoiceCount(int n) {
    if (n < 1)
        n = 1;
    if (n == target_voice_count_)
        return;
    EnqueueAction([this, n]() {
        target_voice_count_ = n;
        if (!master_.expired())
            RebuildReplicas(n);
    });
}

void Voicebank::RebuildReplicas(int n) {
    auto master = master_.lock();

    // Remove existing replica children (replicas_ is cleared via
    // OnRemovingVoice)
    while (!replicas_.empty())
        replicas_.back()->Destroy();

    if (!master)
        return;

    // Serialize master once, reuse for each replica.
    nlohmann::json master_json;
    Archive master_archive(master_json);
    ArchiverManufacturer::singleton().Serialize(master_archive, *master);

    replicas_.reserve(n);
    for (int i = 0; i < n; ++i) {
        auto v = Model::Make<Voice>(shared_from_this(), CreateMode::Replicated);
        ArchiverManufacturer::singleton().Deserialize(master_archive, *v);
    }
    for (auto &replica : replicas_) {
        replica->LinkTo(*master);
    }
}

void Voicebank::Process() {
    for (const MidiMessage &msg : midi_in_module_->midi_in_->Drain())
        HandleMidi(msg);

    Subrack::Process();

    SumReplicasIntoOutput();
}

void Voicebank::HandleMidi(const MidiMessage &msg) {
    if (replicas_.empty())
        return;

    if (msg.IsNoteOn()) {
        if (msg.velocity() == 0)
            HandleNoteOff(msg);
        else
            HandleNoteOn(msg);
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
    Voice *v = replicas_[idx];
    v->set_current_note(msg.key());
    v->set_note_on_tick(next_tick_++);
    v->DeliverMidi(msg);
}

void Voicebank::HandleNoteOff(const MidiMessage &msg) {
    for (auto *v : replicas_) {
        if (v->current_note() == msg.key()) {
            v->DeliverMidi(msg);
            v->set_current_note(-1);
        }
    }
}

void Voicebank::BroadcastToAllVoices(const MidiMessage &msg) {
    for (auto *v : replicas_)
        v->DeliverMidi(msg);
}

bool Voicebank::VoiceIsFree(const Voice &v) const {
    return v.current_note() < 0 && !v.IsActive();
}

int Voicebank::AllocateVoiceForNote(int note) {
    if (replicas_.empty())
        return -1;

    // 1. Same-note retrigger.
    for (size_t i = 0; i < replicas_.size(); ++i) {
        if (replicas_[i]->current_note() == note && replicas_[i]->IsActive())
            return static_cast<int>(i);
    }

    // 2. A free voice.
    for (size_t i = 0; i < replicas_.size(); ++i) {
        if (VoiceIsFree(*replicas_[i]))
            return static_cast<int>(i);
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

void Voicebank::SumReplicasIntoOutput() {
    if (replicas_.empty()) {
        audio_out_module_->audio_out_->Write(Audio());
        return;
    }

    Audio mixed = replicas_[0]->audio_out_->Read();

    if (mixed.Empty()) {
        audio_out_module_->audio_out_->Write(Audio());
        return;
    }

    fy_real *d = mixed.array().data();
    const size_t n = mixed.array().size();

    for (size_t i = 1; i < replicas_.size(); ++i) {
        const Audio voice = replicas_[i]->audio_out_->Read();
        const fy_real *s = voice.array().data();
        for (size_t k = 0; k < n; ++k)
            d[k] += s[k];
    }

    for (size_t k = 0; k < n; ++k)
        d[k] = std::tanh(d[k]);

    audio_out_module_->audio_out_->Write(mixed);
}

void Voicebank::OnMasterParameterChanged() {
    // Deferred.
}

} // namespace augr

using namespace augr;
DEFINE_MODEL_FACTORY(Voicebank, "Voicebank", "Polyphony")