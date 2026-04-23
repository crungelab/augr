// augr/fm/voice_bank.cpp

#include <cmath>

#include <augr/fm/voice_bank.h>

namespace augr::fm {

namespace {

// MIDI note number to Hz. Matches the Hz-based pitch convention from Operator.
// If you migrate to V/oct, this becomes `(note - 69) / 12.0f + 4.75f`.
float NoteToHz(std::uint8_t note) {
    return 440.0f * std::pow(2.0f, (static_cast<int>(note) - 69) / 12.0f);
}

} // namespace

template <std::size_t NV, std::size_t OP>
bool VoiceBank<NV, OP>::Create(Part &owner) {
    if (!Module::Create(owner)) return false;

    midi_in_   = owner.Add<MidiInput>(*this, "midi");
    audio_out_ = owner.Add<AudioOutput>(*this, "out", ChannelLayout::kMono);

    for (std::size_t i = 0; i < NV; ++i) {
        pitch_outs_[i] = owner.Add<VoltageOutput>(*this, "pitch_" + std::to_string(i));
        gate_outs_[i]  = owner.Add<VoltageOutput>(*this, "gate_"  + std::to_string(i));

        voices_[i] = owner.Add<Voice<OP>>();
        voices_[i]->Create(owner);
        voices_[i]->cv_pitch_in_->Connect(*pitch_outs_[i]);
        voices_[i]->gate_in_->Connect(*gate_outs_[i]);
    }
    return true;
}

template <std::size_t NV, std::size_t OP>
void VoiceBank<NV, OP>::Process() {
    // Drain MIDI events for this buffer. A more sophisticated implementation
    // would sample-accurately schedule note-on/off within the buffer; this
    // version applies them at buffer boundaries, which is fine for most use.
    for (const auto &ev : midi_in_->Events()) {
        switch (ev.type) {
        case MidiEvent::kNoteOn:
            if (ev.velocity > 0) NoteOn(ev.note, ev.velocity);
            else                 NoteOff(ev.note);
            break;
        case MidiEvent::kNoteOff:
            NoteOff(ev.note);
            break;
        default: break;
        }
    }

    const std::size_t frames = audio_out_->Frames();

    // Write per-voice pitch and gate buffers from slot state. Constant across
    // the buffer; a future pitch-bend / portamento pass would shape them.
    for (std::size_t i = 0; i < NV; ++i) {
        Audio &pb = pitch_outs_[i]->Buffer();
        Audio &gb = gate_outs_[i]->Buffer();
        const float hz   = slots_[i].active ? NoteToHz(slots_[i].note) : 0.0f;
        const float gate = slots_[i].active ? 1.0f : 0.0f;
        for (std::size_t f = 0; f < frames; ++f) {
            pb(0, f) = hz;
            gb(0, f) = gate;
        }
    }

    // Process all voices and sum into the bank output.
    Audio &out = audio_out_->Buffer();
    out.Zero();
    const float gain = 1.0f / static_cast<float>(NV);  // naive headroom
    for (std::size_t i = 0; i < NV; ++i) {
        voices_[i]->Process();
        const Audio &vb = voices_[i]->audio_out_->Buffer();
        for (std::size_t f = 0; f < frames; ++f) {
            out(0, f) += vb(0, f) * gain;
        }
    }
}

template <std::size_t NV, std::size_t OP>
void VoiceBank<NV, OP>::Update(float deltaTime) {
    for (auto *v : voices_) if (v) v->Update(deltaTime);
}

template <std::size_t NV, std::size_t OP>
void VoiceBank<NV, OP>::LoadPatch(const Patch<OP> &patch) {
    for (auto *v : voices_) if (v) ApplyPatch(patch, *v);
}

template <std::size_t NV, std::size_t OP>
std::size_t VoiceBank<NV, OP>::AllocateSlot() {
    // Prefer an idle slot. If none, steal the oldest active voice — standard
    // LRU stealing. More musical policies (steal quietest, steal same-note)
    // are easy extensions once the slot array carries amplitude info.
    std::size_t idle_idx = NV;
    std::size_t oldest_idx = 0;
    std::uint32_t oldest_age = UINT32_MAX;

    for (std::size_t i = 0; i < NV; ++i) {
        if (!slots_[i].active && idle_idx == NV) idle_idx = i;
        if (slots_[i].age < oldest_age) {
            oldest_age = slots_[i].age;
            oldest_idx = i;
        }
    }
    return idle_idx < NV ? idle_idx : oldest_idx;
}

template <std::size_t NV, std::size_t OP>
void VoiceBank<NV, OP>::NoteOn(std::uint8_t note, std::uint8_t velocity) {
    const std::size_t i = AllocateSlot();
    slots_[i] = Slot{note, velocity, ++event_counter_, true};
}

template <std::size_t NV, std::size_t OP>
void VoiceBank<NV, OP>::NoteOff(std::uint8_t note) {
    // Release the most recently triggered voice holding this note.
    // Voices with envelopes still in their release stage will continue
    // producing output until the envelope reaches idle — that's handled by
    // the envelope itself, not the slot.
    std::size_t target = NV;
    std::uint32_t best_age = 0;
    for (std::size_t i = 0; i < NV; ++i) {
        if (slots_[i].active && slots_[i].note == note && slots_[i].age >= best_age) {
            best_age = slots_[i].age;
            target = i;
        }
    }
    if (target < NV) slots_[target].active = false;
}

// Explicit instantiations for common configurations.
template class VoiceBank<8, 6>;
template class VoiceBank<16, 6>;
template class VoiceBank<8, 4>;

} // namespace augr::fm