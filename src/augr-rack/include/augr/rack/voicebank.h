#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include <augr/rack/subrack.h>

namespace augr {

class Voice;
class MidiInputModule;
class AudioOutputModule;
class MidiMessage;

// Voicebank: a polyphonic container holding one editable master Voice
// (visible in the graph editor) and N hidden replicas cloned from it.
// MIDI flows in -> note allocation -> per-voice replicas -> audio sum
// out.
//
// The voicebank is itself a Subrack so the user gets the editor for
// free and can add post-voice FX inside it. The master Voice lives in
// modules_ like any child; replicas live in a separate vector and are
// not part of the graph -- they are an audio-thread implementation
// detail, not user content.
class Voicebank : public Subrack {
public:
    Voicebank() = default;

    void Create(Model *parent) override;

    void Process() override;

    // -- Voice management ----------------------------------------------
    // Set polyphony. Re-clones replicas from master; cuts off any
    // active notes. Routes through EnqueueAction so audio thread stays
    // safe.
    void SetVoiceCount(int n);
    int  voice_count() const { return static_cast<int>(replicas_.size()); }

    Voice *master() { return master_; }

    // -- Parameter replication -----------------------------------------
    // Called when a parameter on the master changes. Fans the change
    // out to all replicas at the same parameter path. Does NOT
    // re-clone -- active notes survive parameter edits.
    //
    // TODO: signature depends on how augr-core represents parameter
    // paths + values. Placeholder for now.
    void OnMasterParameterChanged(/* path, value */);

    REFLECT_ENABLE(Subrack)

private:
    // -- MIDI dispatch -------------------------------------------------
    void HandleMidi(const MidiMessage &msg);
    void HandleNoteOn(const MidiMessage &msg);
    void HandleNoteOff(const MidiMessage &msg);
    void BroadcastToAllVoices(const MidiMessage &msg);

    // -- Voice allocation ---------------------------------------------
    // Returns index into replicas_, or -1 if allocation failed.
    // Priority: same-note retrigger, free voice, steal oldest active.
    int AllocateVoiceForNote(int note);

    // -- Cloning -------------------------------------------------------
    // Serializes master to JSON and deserializes n replicas. Replaces
    // any existing replicas. Audio-thread unsafe; call via
    // EnqueueAction.
    void RebuildReplicas(int n);

    // -- Audio summing -------------------------------------------------
    void SumReplicasIntoOutput();

    // IoModules that expose the voicebank's outer pins. These are
    // children in modules_, just like in Voice. The user can wire
    // post-voice FX between voice outputs and audio_out_module_.
    MidiInputModule   *midi_in_module_   = nullptr;
    AudioOutputModule *audio_out_module_ = nullptr;

    // The editable template. Owned by Subrack's modules_ as a child;
    // we keep a typed pointer for convenience.
    Voice *master_ = nullptr;

    // Played voices. Not children of the subrack -- owned here. Their
    // Process() is called directly by Voicebank::Process(), not via
    // the graph's topological walk.
    std::vector<std::unique_ptr<Voice>> replicas_;

    // Monotonic counter for voice-stealing (oldest = lowest tick).
    uint64_t next_tick_ = 1;
};

} // namespace augr