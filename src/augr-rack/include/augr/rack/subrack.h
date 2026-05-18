#pragma once

#include <algorithm>
#include <functional>
#include <vector>

#include <augr/core/midi/midi_message.h>

#include <augr/rack/graph.h>

namespace augr {

class Module;
class Device;
class AudioInputDevice;
class AudioOutputDevice;
class MidiInputDevice;
class MidiOutputDevice;
class Rack;

class Subrack : public Graph {
public:
    Subrack() = default;

    void Create(Model *parent) override {
        Graph::Create(parent);
        label_ = "Subrack";
    }

    // -- Execution ------------------------------------------------------
    // Rebuilds execution order if dirty, then walks sorted_modules_
    // in topological order calling Process() on each.
    void Process() override;

    // Topological sort over modules_ using outport wires.
    // Non-Module Nodes (e.g. WDF ports) participate in topology
    // but are not scheduled here.
    void RebuildExecutionOrder();

    // -- Child management ----------------------------------------------
    // Adds Module / Device branches on top of Graph's wire+pin bookkeeping.
    void OnAddingChild(Model &model) override;
    void OnRemovingChild(Model &model) override;
    void OnAddingDevice(Device &device);
    void OnRemovingDevice(Device &device);

    // -- Action queue interface ----------------------------------------
    // These walk up parents to the outer Rack and forward.
    // Defined in subrack.cpp to avoid including rack.h here.
    void EnqueueAction(std::function<void()> action,
                       std::function<void()> update_action = nullptr);
    void EnqueueUpdateAction(std::function<void()> action);

    // Called from the MIDI thread; routes to the outer Rack's queue.
    void EnqueueMidiMessage(MidiMessage message);

    // -- Accessors -----------------------------------------------------
    // Walks parents to find the enclosing Rack (returns nullptr if none).
    Rack *OuterRack();

    // Data members --- scheduling
    std::vector<Module *> modules_;
    std::vector<Module *> sorted_modules_; // cached execution order

    // Data members --- devices (virtual / boundary modules)
    AudioInputDevice *audio_input_device_ = nullptr;
    AudioOutputDevice *audio_output_device_ = nullptr;
    MidiInputDevice *midi_input_device_ = nullptr;
    MidiOutputDevice *midi_output_device_ = nullptr;

    REFLECT_ENABLE(Graph)

protected:
    bool CreateAudioInputDevice();
    bool CreateAudioOutputDevice();
    bool CreateMidiInputDevice();
    bool CreateMidiOutputDevice();

private:
    void AddModule(Module &m) { modules_.push_back(&m); }
    void RemoveModule(Module &m) {
        modules_.erase(std::remove(modules_.begin(), modules_.end(), &m),
                       modules_.end());
    }
};

} // namespace augr