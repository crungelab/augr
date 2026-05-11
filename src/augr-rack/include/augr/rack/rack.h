#pragma once

#include <functional>

#include <augr/core/midi/midi_message.h>

#include <augr/rack/graph.h>
#include <augr/rack/rack_config.h>

namespace augr {

class Module;
class Device;
class AudioInputDevice;
class AudioOutputDevice;
class MidiInputDevice;
class MidiOutputDevice;

class Rack : public Graph {
public:
    Rack() { singleton_ = this; }
    virtual ~Rack();
    //
    void CreateDefaultDevices();
    //
    void OnAddingChild(Model &model) override;
    void OnAddingDevice(Device &device);

    void OnRemovingChild(Model &model) override;
    void OnRemovingDevice(Device &device);


    void EnqueueAction(std::function<void()> action,
                       std::function<void()> update_action = nullptr);
    void EnqueueUpdateAction(std::function<void()> action);

    // Called by MidiSystem::Callback on the MIDI thread;
    // enqueues into the action queue for processing on the audio thread
    void EnqueueMidiMessage(MidiMessage message);

    void ProcessActions();
    void ProcessUpdateActions();

    void RebuildExecutionOrder();
    //
    virtual bool Start() { running_ = true; return true; }
    virtual void Stop() { running_ = false; }
    // Accessors
    static Rack &singleton() { return *singleton_; }
    bool IsRunning() const { return running_; }
    // Data members
    static Rack *singleton_;
    RackConfig config_;
    std::vector<Module *> modules_;
    std::vector<Module *> sorted_modules_; // cached execution order
    //
    std::mutex mutex_;
    std::vector<std::function<void()>> pending_actions_;
    std::vector<std::function<void()>> pending_update_actions_;
    //
    // Data members
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
    // Data members
    bool running_ = false;
};

} // namespace augr