#pragma once

#include <algorithm>
#include <vector>

#include <augr/core/audio.h>
#include <augr/core/midi/midi_message.h>

#include <augr/rack/midi_pin.h>
#include <augr/rack/module/module.h>
#include <augr/rack/node.h>
#include <augr/rack/voltage_pin.h>

namespace augr {

class MidiCvModule : public Module {
public:
    void Create(Part *owner = nullptr) override {
        Module::Create(owner);
        label_ = "MIDI/CV";
    }

    void CreatePins() override {
        midi_in_ = new MidiInput(*this, "midi_in");
        AddInput(*midi_in_);

        pitch_out_ = new VoltageOutput(*this, "pitch_out");
        AddOutput(*pitch_out_);

        gate_out_ = new VoltageOutput(*this, "gate_out");
        AddOutput(*gate_out_);

        velocity_out_ = new VoltageOutput(*this, "velocity_out");
        AddOutput(*velocity_out_);
    }

    void Process() override {
        // Drain all pending MIDI messages. This allows us to handle multiple
        // messages per buffer, which is important for high-velocity playing
        for (const MidiMessage &msg : midi_in_->Drain()) {
            HandleMessage(msg);
        }

        const int nFrames = Audio::frames();

        Audio pitch_audio(ChannelLayout::kMono);
        Audio gate_audio(ChannelLayout::kMono);
        Audio velocity_audio(ChannelLayout::kMono);

        fy_real *pitch_buf = pitch_audio.array().data();
        fy_real *gate_buf = gate_audio.array().data();
        fy_real *velocity_buf = velocity_audio.array().data();

        for (int i = 0; i < nFrames; ++i) {
            pitch_buf[i] = pitch_cv_;
            gate_buf[i] = gate_cv_;
            velocity_buf[i] = velocity_cv_;
        }

        pitch_out_->Write(pitch_audio);
        gate_out_->Write(gate_audio);
        velocity_out_->Write(velocity_audio);
    }

    REFLECT_ENABLE(Module)

private:
    void HandleMessage(const MidiMessage &msg) {
        if (msg.IsNoteOn()) {
            // Push onto held-note stack. Last note wins.
            held_notes_.push_back(msg.key());
            pitch_cv_ = KeyToCv(msg.key());
            velocity_cv_ = msg.velocity() / 127.f;
            gate_cv_ = 1.f;
        } else if (msg.IsNoteOff()) {
            // Remove this key from the stack (from the back — handles
            // duplicates).
            auto it =
                std::find(held_notes_.rbegin(), held_notes_.rend(), msg.key());
            if (it != held_notes_.rend()) {
                held_notes_.erase(std::next(it).base());
            }

            if (held_notes_.empty()) {
                gate_cv_ = 0.f;
                // Leave pitch_cv_ and velocity_cv_ alone — the ADSR is in
                // release; a sudden pitch jump mid-release would be audible.
            } else {
                // Fall back to the previous held note without re-triggering the
                // gate.
                pitch_cv_ = KeyToCv(held_notes_.back());
            }
        }
        // Other message types ignored in v1.
    }

    // MIDI note 60 (C4) = 0V, 1V/octave.
    static float KeyToCv(uint8_t key) {
        return (static_cast<int>(key) - 60) / 12.f;
    }

    MidiInput *midi_in_ = nullptr;
    VoltageOutput *pitch_out_ = nullptr;
    VoltageOutput *gate_out_ = nullptr;
    VoltageOutput *velocity_out_ = nullptr;

    std::vector<uint8_t> held_notes_;
    float pitch_cv_ = 0.f;
    float gate_cv_ = 0.f;
    float velocity_cv_ = 0.f;
    double last_seconds_ = -1.0;
};

} // namespace augr