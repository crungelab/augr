#include <augr/core/model_factory.h>
#include <augr/core/archiver_factory.h>

#include <augr/rack/rack.h>

#include <augr/rack/module/audio_device.h>
#include <augr/rack/module/midi_device.h>

namespace augr {

Rack *Rack::singleton_;

Rack::~Rack() {
    Stop();
}

// ---------------------------------------------------------------------------
// Default device set
// ---------------------------------------------------------------------------

void Rack::OnFresh() {
    if (config_.audio_input_channels > 0)
        CreateAudioInputDevice();
    CreateAudioOutputDevice();
    if (config_.enable_midi_input)
        CreateMidiInputDevice();
    if (config_.enable_midi_output)
        CreateMidiOutputDevice();
}

// ---------------------------------------------------------------------------
// Action queue
// ---------------------------------------------------------------------------

void Rack::EnqueueAction(std::function<void()> action,
                         std::function<void()> update_action) {
    std::lock_guard lock(mutex_);
    pending_actions_.push_back(std::move(action));
    if (update_action) {
        pending_update_actions_.push_back(std::move(update_action));
    }
}

void Rack::EnqueueUpdateAction(std::function<void()> action) {
    std::lock_guard lock(mutex_);
    pending_update_actions_.push_back(std::move(action));
}

void Rack::ProcessActions() {
    std::vector<std::function<void()>> actions;
    {
        std::lock_guard lock(mutex_);
        std::swap(actions, pending_actions_);
    }
    for (auto &a : actions)
        a();
}

void Rack::ProcessUpdateActions() {
    std::vector<std::function<void()>> actions;
    {
        std::lock_guard lock(mutex_);
        std::swap(actions, pending_update_actions_);
    }
    for (auto &a : actions)
        a();
}

// ---------------------------------------------------------------------------
// Child management
// ---------------------------------------------------------------------------

void Rack::OnAddingChild(Model &model) {
    Subrack::OnAddingChild(model);

    if (auto *d = dynamic_cast<Device *>(&model)) {
        OnAddingDevice(*d);
    }
}

void Rack::OnRemovingChild(Model &model) {
    if (auto *d = dynamic_cast<Device *>(&model)) {
        OnRemovingDevice(*d);
    }
    Subrack::OnRemovingChild(model);
}

void Rack::OnAddingDevice(Device &device) {
    if (auto *audio_input = dynamic_cast<AudioInputDevice *>(&device)) {
        audio_input_device_ = audio_input;
    } else if (auto *audio_output =
                   dynamic_cast<AudioOutputDevice *>(&device)) {
        audio_output_device_ = audio_output;
    } else if (auto *midi_input = dynamic_cast<MidiInputDevice *>(&device)) {
        midi_input_device_ = midi_input;
    } else if (auto *midi_output = dynamic_cast<MidiOutputDevice *>(&device)) {
        midi_output_device_ = midi_output;
    }
}

void Rack::OnRemovingDevice(Device &device) {
    if (audio_input_device_ == &device)
        audio_input_device_ = nullptr;
    if (audio_output_device_ == &device)
        audio_output_device_ = nullptr;
    if (midi_input_device_ == &device)
        midi_input_device_ = nullptr;
    if (midi_output_device_ == &device)
        midi_output_device_ = nullptr;
}

// ---------------------------------------------------------------------------
// Device construction
// ---------------------------------------------------------------------------

bool Rack::CreateAudioInputDevice() {
    audio_input_device_ = ModelFactoryT<AudioInputDevice>::Make(this);
    return true;
}

bool Rack::CreateAudioOutputDevice() {
    audio_output_device_ = ModelFactoryT<AudioOutputDevice>::Make(this);
    return true;
}

bool Rack::CreateMidiInputDevice() {
    midi_input_device_ = ModelFactoryT<MidiInputDevice>::Make(this);
    return true;
}

bool Rack::CreateMidiOutputDevice() {
    midi_output_device_ = ModelFactoryT<MidiOutputDevice>::Make(this);
    return true;
}

void Rack::EnqueueMidiMessage(MidiMessage message) {
    EnqueueAction([this, message = std::move(message)]() {
        if (midi_input_device_) {
            midi_input_device_->midi_out_->Write(message);
        }
    });
}


} // namespace augr