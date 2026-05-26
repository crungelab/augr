#include <augr/core/archiver_factory.h>
#include <augr/core/model_factory.h>
#include <augr/core/midi/midi_message.h>

#include <augr/rack/archiver/module_archiver.h>
#include <augr/rack/module/midi_io.h>

#include <augr/rack/graph.h>

namespace augr {

void MidiInputModule::Create(Model *parent) {
    Io::Create(parent);
    label_ = "Midi Input Module";
}

void MidiInputModule::CreatePins() {
    midi_out_ = new MidiOutput(*this, "midi_out");
    AddOutput(*midi_out_);

    midi_in_ = new MidiInput(*this, "midi_in");
    graph().AddInput(*midi_in_);
}

void MidiInputModule::Process() {
    for (const MidiMessage &msg : midi_in_->Drain()) {
        midi_out_->Write(msg);
    }
}

void MidiOutputModule::Create(Model *parent) {
    Io::Create(parent);
    label_ = "Midi Output Module";
}

void MidiOutputModule::CreatePins() {
    midi_in_ = new MidiInput(*this, "midi_in");
    AddInput(*midi_in_);

    midi_out_ = new MidiOutput(graph(), "midi_out");
    graph().AddOutput(*midi_out_);
}

void MidiOutputModule::Process() {
    for (const MidiMessage &msg : midi_in_->Drain()) {
        midi_out_->Write(msg);
    }
}

} // namespace augr

using namespace augr;

DEFINE_MODULE(MidiInputModule, "MidiInputModule", "Io")
DEFINE_MODULE(MidiOutputModule, "MidiOutputModule", "Io")
