#include <augr/rack/voicebank.h>

#include <algorithm>

#include <augr/core/model_factory.h>

#include <augr/core/archiver.h>
#include <augr/core/archiver_factory.h>
#include <augr/core/archiver_manufacturer.h>

#include <augr/core/midi/midi_message.h>

#include <augr/rack/module/audio_io.h>
#include <augr/rack/module/midi_io.h>
#include <augr/rack/voice.h>

namespace augr {

void Voicebank::Create(Model *parent) {
    Subrack::Create(parent);
    label_ = "Voicebank";

    // Outer-facing IoModules. The user can wire things between
    // voice replicas (summed) and audio_out_module_ for post-FX.
    midi_in_module_ = new MidiInputModule();
    midi_in_module_->Create(this);

    audio_out_module_ = new AudioOutputModule();
    audio_out_module_->Create(this);

    // The master voice: editable in the graph, processed for
    // monitoring/scope use but its audio output is not summed into
    // the voicebank output.
    master_ = new Voice();
    master_->Create(this);

    // Default polyphony. The audio thread is not running yet during
    // Create, so we can RebuildReplicas directly instead of through
    // EnqueueAction.
    RebuildReplicas(8);
}

void Voicebank::SetVoiceCount(int n) {
    if (n < 1)
        n = 1;
    if (n == voice_count())
        return;

    // Audio-thread safe: defer the actual rebuild.
    EnqueueAction([this, n]() { RebuildReplicas(n); });
}

void Voicebank::RebuildReplicas(int n) {
    // ASSUMPTION: augr-core has a way to serialize a module subtree
    // to JSON and deserialize it back. Based on the memory of the
    // Archive/Archiver system, the rough shape is:
    //
    //   nlohmann::json j;
    //   Archive ar(j);
    //   ar.SerializeModule(master_);
    //   ...
    //   auto replica = std::make_unique<Voice>();
    //   replica->Create(this);  // or whatever parent makes sense
    //   ar.DeserializeModule(replica.get(), j);
    //
    // The exact API is yours; this is the placeholder.
    //
    // Important: replicas are NOT added to modules_, so they do not
    // appear in the graph editor and do not participate in the
    // subrack's topological sort. They're driven manually below.

    replicas_.clear();
    replicas_.reserve(n);

    // Serialize master once, reuse the json for each replica.
    nlohmann::json master_json;
    Archive master_archive(master_json);
    ArchiverManufacturer::singleton().Serialize(master_archive, *master_);

    for (int i = 0; i < n; ++i) {
        auto v = std::make_unique<Voice>();
        // v->Create(this);
        v->Create();
        // DeserializeVoiceFromJson(v.get(), master_json);
        ArchiverManufacturer::singleton().Deserialize(master_archive, *v.get());
        replicas_.push_back(std::move(v));
    }
}

void Voicebank::Process() {
    // 1. Drain MIDI input and dispatch.
    //
    // ASSUMPTION: MidiInputModule exposes its inner-graph MidiOutput
    // pin and we can read drained messages from somewhere accessible
    // on the IoModule. In your MidiInputModule::Process() you call
    // midi_in_->Drain() and forward to midi_out_. For the voicebank
    // we want to *intercept* the MIDI stream before it reaches the
    // master/replicas' wired internals.
    //
    // Cleanest option: the voicebank reads directly from the outer
    // midi_in_ pin (same one MidiInputModule reads in its Process),
    // and dispatches per-voice. The IoModule's normal forwarding
    // still happens for the master (for scope/monitoring), and for
    // any internal wiring inside the voicebank subrack. Each replica
    // gets MIDI delivered via its own MidiInputModule's input pin.
    //
    // Concretely, I'm assuming midi_in_module_->midi_in_ gives us
    // the outer pin and we can iterate its pending messages without
    // draining (so the IoModule's normal drain still works for the
    // master path). If your MidiInput only supports destructive
    // Drain(), we'd need to drain here and re-inject into the
    // master's MidiInputModule. Flag for you to confirm.

    for (const MidiMessage &msg : midi_in_module_->midi_in_->Drain()) {
        HandleMidi(msg);
    }

    // 2. Process the subrack itself. This runs IoModules, the master
    //    (for monitoring), and any post-voice FX the user added.
    Subrack::Process();

    // 3. Process replicas. They're not in the subrack's topology, so
    //    we walk them manually. Each replica is self-contained (its
    //    own subrack), so order doesn't matter between replicas.
    for (auto &v : replicas_) {
        v->Process();
    }

    // 4. Sum replica audio outputs into our audio output.
    SumReplicasIntoOutput();
}

void Voicebank::HandleMidi(const MidiMessage &msg) {
    // ASSUMPTION: MidiMessage exposes IsNoteOn(), IsNoteOff(), Note(),
    // Velocity(). Names are best-guess; adjust.
    if (msg.IsNoteOn()) {
        // Velocity 0 note-on is conventionally a note-off.
        if (msg.velocity() == 0) {
            HandleNoteOff(msg);
        } else {
            HandleNoteOn(msg);
        }
    } else if (msg.IsNoteOff()) {
        HandleNoteOff(msg);
    } else {
        // CC, pitch bend, channel pressure, etc. -> all voices.
        BroadcastToAllVoices(msg);
    }
}

void Voicebank::HandleNoteOn(const MidiMessage &msg) {
    int idx = AllocateVoiceForNote(msg.key());
    if (idx < 0)
        return;

    Voice *v = replicas_[idx].get();

    // ASSUMPTION: Voice exposes a way to inject a MIDI message into
    // its own midi_in_module_, or has NoteOn(note, velocity) helpers.
    // The cleanest route is to deliver the actual MidiMessage so the
    // voice's internal MidiToCv (or whatever the user wired) handles
    // it uniformly. Sketching as DeliverMidi:
    //
    // v->DeliverMidi(MidiMessage::NoteOn(note, velocity));
    //
    // It also needs voicebank-side bookkeeping for stealing:
    // v->SetCurrentNote(note);
    // v->SetNoteOnTick(next_tick_++);

    v->midi_in_->Write(msg);
}

void Voicebank::HandleNoteOff(const MidiMessage &msg) {
    // Find the replica(s) currently playing this note and send
    // note-off. There may be more than one (same-note retrigger
    // without release, depending on stealing policy) -- send to all.
    for (auto &v : replicas_) {
        if (v->CurrentNote() == msg.key()) {
            v->midi_in_->Write(msg);
            // v->DeliverMidi(MidiMessage::NoteOff(note));
            // The voice's internal envelope sees gate fall and
            // starts release. IsActive() will return true until
            // release completes (or fixed timeout, depending on
            // whether done_out_ is wired internally).
        }
    }
}

void Voicebank::BroadcastToAllVoices(const MidiMessage &msg) {
    for (auto &v : replicas_) {
        v->midi_in_->Write(msg);
    }
}

int Voicebank::AllocateVoiceForNote(int note) {
    if (replicas_.empty())
        return -1;

    // 1. Same-note retrigger: a replica already playing this note.
    //    Reuse it -- this matches how most synths behave for repeated
    //    notes (legato/retrigger depends on the envelope, not voice
    //    allocation).
    for (size_t i = 0; i < replicas_.size(); ++i) {
        if (replicas_[i]->CurrentNote() == note && replicas_[i]->IsActive()) {
            return static_cast<int>(i);
        }
    }

    // 2. A free voice (IsActive() == false).
    for (size_t i = 0; i < replicas_.size(); ++i) {
        if (!replicas_[i]->IsActive()) {
            return static_cast<int>(i);
        }
    }

    // 3. Steal: the replica with the oldest NoteOnTick().
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
    // ASSUMPTION: AudioOutputModule's outer pin is a buffer we write
    // into, and Voice's audio_out_ pin has a readable buffer per
    // block. Channel counts must match (or we handle stereo<->mono
    // upmix here -- punt for now and assume the voice stereo matches
    // the voicebank stereo).
    //
    // Pseudocode:
    //
    //   auto &out = audio_out_module_->audio_out_->buffer();
    //   out.Zero();
    //   for (auto &v : replicas_) {
    //       const auto &voice_buf = v->audio_out_->buffer();
    //       out.AddFrom(voice_buf);
    //   }
    //
    // If you have a buffer-pool with raw pointers (per the recent
    // direction in your audio buffer work), this becomes a straight
    // loop over fy_real* with += per sample.
    for (auto &v : replicas_) {
        auto audio = v->audio_out_->Read();
        audio_out_module_->audio_out_->Write(audio);
    }
}

void Voicebank::OnMasterParameterChanged(/* path, value */) {
    // ASSUMPTION: when the user changes a parameter on the master,
    // something in the editor calls this. We fan out the same change
    // to each replica at the same path. Audio thread safety: this
    // is called on the UI thread, so we route the actual replica
    // mutation through EnqueueAction.
    //
    // EnqueueAction([this, path, value]() {
    //     for (auto &v : replicas_) {
    //         v->SetParameterByPath(path, value);
    //     }
    // });
    //
    // The path mechanism is the same one the archiver uses to address
    // parameters during serialization. If that machinery isn't
    // exposed for runtime address-and-set, this is where you'd add
    // it.
}

} // namespace augr

using namespace augr;
DEFINE_MODEL_FACTORY(Voicebank, "Voicebank", "Polyphony")