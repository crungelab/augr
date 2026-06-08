#pragma once

#include <augr/rack/subrack.h>

namespace augr {

class Io;
class MidiInputModule;
class AudioOutputModule;
class CvOutputModule;

class Voice : public Subrack {
public:
    Voice() = default;
    void OnDestroy() override;

    void Create() override;
    void OnFresh() override;
    void OnLoaded() override;

    // -- Child management ----------------------------------------------
    void OnAddingIo(Io &io) override;
    void OnRemovingIo(Io &io) override;

    // -- Polyphony contract --------------------------------------------
    // The voicebank calls this. Default implementation reads done_in_
    // if anything is wired to it; otherwise falls back to a timeout
    // measured from the most recent note-off.
    virtual bool IsActive() const;

    // Currently-assigned MIDI note, or -1 if free.
    int current_note() const { return current_note_; }
    void set_current_note(int note) { current_note_ = note; }
    uint64_t note_on_tick() const { return note_on_tick_; }
    void set_note_on_tick(uint64_t tick) { note_on_tick_ = tick; }

    // Voicebank-facing helpers. Push MIDI into midi_in_ which forwards
    // it to midi_out_ inside the graph.
    void DeliverMidi(const MidiMessage &msg);

    REFLECT_ENABLE(Subrack)

private:
    MidiInputModule * midi_in_module_ = nullptr;

    AudioOutputModule *audio_out_module_ = nullptr;

    CvOutputModule *done_out_module_ = nullptr;
    VoltageOutput *done_out_ = nullptr;

    int current_note_ = -1;
    uint64_t note_on_tick_ = 0;
    int samples_since_note_off_ = 0;
    int release_timeout_samples_ = 96000; // 2 sec @ 48k
};

} // namespace augr