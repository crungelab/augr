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
    void SetMaster(Voice *master);
    Voice *master() const {
        auto sp = master_.lock();
        return sp ? sp.get() : nullptr;
    }

    // -- Voice management ----------------------------------------------
    void SetVoiceCount(int n);
    int voice_count() const { return static_cast<int>(replicas_.size()); }

    // -- Parameter replication -----------------------------------------
    void OnMasterParameterChanged();

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
    void RebuildReplicas(int n);

    // -- Audio summing -------------------------------------------------
    void SumReplicasIntoOutput();

    // Non-owning observers — owned as children by this Subrack.
    MidiInputModule *midi_in_module_ = nullptr;
    AudioOutputModule *audio_out_module_ = nullptr;

    // Weak reference to the master Voice (lives elsewhere in the rack).
    // Automatically becomes null if the master is destroyed, preventing
    // dangling pointer access. Lock before use.
    std::weak_ptr<Voice> master_;

    // Non-owning observers — owned as children by this Subrack.
    std::vector<Voice *> replicas_;

    uint64_t next_tick_ = 1;
    int target_voice_count_ = 8;
};

} // namespace augr