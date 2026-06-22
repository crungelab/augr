#pragma once

#include <functional>
#include <mutex>
#include <vector>

#include <augr/rack/rack_config.h>
#include <augr/rack/subrack.h>

#include <augr/rack/voice/voice_manager.h>

namespace augr {

class Device;
class AudioInputDevice;
class AudioOutputDevice;
class MidiInputDevice;
class MidiOutputDevice;

class Rack : public Subrack {
public:
    Rack() { singleton_ = this; }
    virtual ~Rack();

    // Top-level convenience: creates the standard set of boundary devices
    // based on config_. Delegates to Subrack's Create*Device helpers.
    void OnCreateFresh() override;
    void CreateControls() override;

    // -- Action queue --------------------------------------------------
    // Thread bridge: any thread enqueues, audio thread drains via
    // ProcessActions().
    void EnqueueAction(std::function<void()> action);
    // Called from the MIDI thread; routes to the outer Rack's queue.
    void EnqueueMidiMessage(MidiMessage message);

    void ProcessActions();

    // -- Child management ----------------------------------------------
    // Adds Module / Device branches on top of Graph's wire+pin bookkeeping.
    void OnAddingChild(Model &model) override;
    void OnRemovingChild(Model &model) override;
    void OnAddingDevice(Device &device);
    void OnRemovingDevice(Device &device);

    // -- Lifecycle -----------------------------------------------------
    virtual bool Start() {
        running_ = true;
        return true;
    }
    virtual void Stop() { running_ = false; }

    // -- Accessors -----------------------------------------------------
    static Rack &singleton() { return *singleton_; }
    bool IsRunning() const { return running_; }

    VoiceManager &voice_manager() { return voice_manager_; }

    // Data members
    static Rack *singleton_;
    RackConfig config_;
    VoiceManager voice_manager_;

    std::mutex mutex_;
    std::vector<std::function<void()>> pending_actions_;

    // Data members --- devices (virtual / boundary modules)
    std::shared_ptr<AudioInputDevice> audio_input_device_;
    std::shared_ptr<AudioOutputDevice> audio_output_device_;
    std::shared_ptr<MidiInputDevice> midi_input_device_;
    std::shared_ptr<MidiOutputDevice> midi_output_device_;

    REFLECT_ENABLE(Subrack)

protected:
    bool CreateAudioInputDevice();
    bool CreateAudioOutputDevice();
    bool CreateMidiInputDevice();
    bool CreateMidiOutputDevice();

    float master_volume_ = 1.f;

private:
    bool running_ = false;
};

} // namespace augr