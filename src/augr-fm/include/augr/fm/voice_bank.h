// augr/fm/voice_bank.h
#pragma once

#include <array>
#include <cstdint>
#include <vector>

#include <augr/fm/voice.h>
#include <augr/rack/midi_pin.h>
#include <augr/rack/module.h>

namespace augr::fm {

// Polyphonic voice allocator. Owns N voices of M operators each, routes MIDI
// note-on/off to them via a simple LRU + note-stealing policy, and sums their
// outputs into a mono (or stereo, later) audio output.
//
// Why a Module: the bank is itself a patchable node — its audio_out_ plugs into
// downstream effects, and its midi_in_ receives events from a MIDI source node.
// The per-voice pitch/gate signals are generated internally rather than exposed
// as pins (the bank is the abstraction boundary for polyphony).
template <std::size_t NumVoices = 8, std::size_t OpsPerVoice = 6>
class VoiceBank : public Module {
public:
    bool Create(Part &owner) override;
    void Process() override;
    void Update(float deltaTime) override;

    // Load a patch into all voices. Voices share a patch; per-voice variation
    // (unison detune, random, etc.) is a future extension.
    void LoadPatch(const Patch<OpsPerVoice> &patch);

    std::array<Voice<OpsPerVoice> *, NumVoices> voices_{};

    // midi_in_ reused from Module base
    // audio_out_ reused from Module base

    REFLECT_ENABLE(Module)

private:
    // Per-voice allocation state. Kept small and POD for cache friendliness —
    // voice stealing walks this array every note-on.
    struct Slot {
        std::uint8_t note      = 0;
        std::uint8_t velocity  = 0;
        std::uint32_t age      = 0;    // incremented on every note event
        bool          active   = false;
    };

    void NoteOn(std::uint8_t note, std::uint8_t velocity);
    void NoteOff(std::uint8_t note);
    std::size_t AllocateSlot();  // returns index, stealing if necessary

    // Per-voice pitch/gate buffers live in the bank; each voice's pitch/gate
    // inputs are wired to these on Create().
    std::array<VoltageOutput *, NumVoices> pitch_outs_{};
    std::array<VoltageOutput *, NumVoices> gate_outs_{};

    std::array<Slot, NumVoices> slots_{};
    std::uint32_t event_counter_ = 0;
};

} // namespace augr::fm