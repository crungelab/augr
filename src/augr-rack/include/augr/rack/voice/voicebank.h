#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include <augr/rack/subrack.h>

namespace augr {

class Io;
class MidiInputModule;
class AudioOutputModule;
class MidiMessage;
class Voice;

// Voicebank: a polyphony driver.
//
// The voicebank itself does NOT own a master voice. The user creates
// Voice modules elsewhere in the rack and points one or more
// voicebanks at them via SetMaster() (typically driven by an
// inspector dropdown).
//
// On SetMaster(), the voicebank clones the referenced Voice N times
// into hidden, detached replicas. MIDI flows in, gets dispatched to
// replicas via note allocation, and replica audio is summed into the
// voicebank's audio output.
//
// Voicebank is a Subrack so the user can wire post-voice FX inside it
// (between the summed voices and the audio output).
class Voicebank : public Subrack {
public:
    Voicebank() = default;

    void Create() override;
    void OnFresh() override;

    // -- Child management ----------------------------------------------
    void OnAddingChild(Model &model) override;
    void OnRemovingChild(Model &model) override;
    void OnAddingIo(Io &io) override;
    void OnRemovingIo(Io &io) override;
    void OnAddingVoice(Voice &voice);
    void OnRemovingVoice(Voice &voice);

    void Process() override;

    // -- Master selection ----------------------------------------------
    // Point this voicebank at a Voice elsewhere in the rack. Triggers
    // a replica rebuild. Passing nullptr clears the master; the
    // voicebank then sits silent until a new master is set.
    //
    // Audio-thread safe: routes the actual rebuild through
    // EnqueueAction.
    void SetMaster(Voice *master);
    Voice *master() { return master_; }

    // Identifier of the currently-referenced master, used for
    // serialization.
    const std::string &master_name() const { return master_name_; }

    // -- Voice management ----------------------------------------------
    // Set polyphony. Re-clones replicas from master; cuts off any
    // active notes. Safe to call before a master is set -- the count
    // is stored and applied when SetMaster() runs.
    void SetVoiceCount(int n);
    int voice_count() const { return static_cast<int>(replicas_.size()); }

    // -- Parameter replication -----------------------------------------
    // Deferred. Will be hooked up once the master-change notification
    // path is decided.
    void OnMasterParameterChanged(/* path, value */);

    REFLECT_ENABLE(Subrack)

private:
    // -- MIDI dispatch -------------------------------------------------
    void HandleMidi(const MidiMessage &msg);
    void HandleNoteOn(const MidiMessage &msg);
    void HandleNoteOff(const MidiMessage &msg);
    void BroadcastToAllVoices(const MidiMessage &msg);

    // -- Voice allocation ---------------------------------------------
    bool VoiceIsFree(const Voice &v) const;
    int AllocateVoiceForNote(int note);

    // -- Cloning -------------------------------------------------------
    // Serializes master_ to JSON and deserializes n replicas. No-op
    // if master_ is null. Audio-thread unsafe; call via EnqueueAction.
    void RebuildReplicas(int n);

    // -- Audio summing -------------------------------------------------
    void SumReplicasIntoOutput();

    // Outer-facing IoModules, owned as children in modules_.
    MidiInputModule *midi_in_module_ = nullptr;
    AudioOutputModule *audio_out_module_ = nullptr;

    // Non-owning reference to the master Voice (which lives somewhere
    // else in the rack). May be null.
    Voice *master_ = nullptr;

    // Serialized identity of the master. The voicebank stores this
    // and resolves to a Voice* on load via a rack walk / registry
    // lookup.
    std::string master_name_;

    // Detached replicas cloned from master_. Not children of any
    // subrack; processed manually by Voicebank::Process().
    std::vector<std::unique_ptr<Voice>> replicas_;

    // Monotonic counter for voice-stealing (oldest = lowest tick).
    uint64_t next_tick_ = 1;

    // Polyphony target. Stored separately from replicas_.size() so
    // SetVoiceCount can be called before a master is set, and so
    // SetMaster knows how many replicas to build.
    int target_voice_count_ = 8;
};

} // namespace augr