#include <augr/archiver_factory.h>
#include <augr/model_factory.h>
#include <augr/ui/ui_builder.h>

#include <augr/rack/rack.h>

#include <augr/rack/module/audio_device.h>
#include <augr/rack/module/midi_device.h>

namespace augr {

Rack *Rack::singleton_;

Rack::~Rack() {
    //Stop();
}

// ---------------------------------------------------------------------------
// Default device set
// ---------------------------------------------------------------------------

void Rack::OnCreateFresh() {
    Subrack::OnCreateFresh();
    if (config_.audio_input_channels > 0)
        CreateAudioInputDevice();
    CreateAudioOutputDevice();
    if (config_.enable_midi_input)
        CreateMidiInputDevice();
    if (config_.enable_midi_output)
        CreateMidiOutputDevice();
}

void Rack::CreateControls() {
    UiBuilder ui(controls_);
    auto windowParam = CreateFloatParameter(
        "Master Volume", ControlMeta::kDefault,
        &master_volume_, 1.f, 0.f, 1.f, 0.01f);
    ui.Knob("Master Volume", windowParam);
}

/*
void Rack::CreateControls() {
    UiBuilder ui(shared_from_this());
    auto windowParam = CreateFloatParameter(
        "Master Volume", ControlMeta::kDefault,
        &master_volume_, 1.f, 0.f, 1.f, 0.01f);
    ui.Knob("Master Volume", windowParam);
}
*/


// ---------------------------------------------------------------------------
// Action queue
// ---------------------------------------------------------------------------

void Rack::EnqueueAction(std::function<void()> action) {
    std::lock_guard lock(mutex_);
    pending_actions_.push_back(std::move(action));
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

// ---------------------------------------------------------------------------
// Child management
// ---------------------------------------------------------------------------

void Rack::OnAddingChild(Model &model) {
    Subrack::OnAddingChild(model);
    if (auto *d = dynamic_cast<Device *>(&model))
        OnAddingDevice(*d);
}

void Rack::OnRemovingChild(Model &model) {
    if (auto *d = dynamic_cast<Device *>(&model))
        OnRemovingDevice(*d);
    Subrack::OnRemovingChild(model);
}

void Rack::OnAddingDevice(Device &device) {
    if (auto *p = dynamic_cast<AudioInputDevice *>(&device))
        audio_input_device_.reset(p,
                                  [](auto *) {}); // non-owning alias — see note
    else if (auto *p = dynamic_cast<AudioOutputDevice *>(&device))
        audio_output_device_.reset(p, [](auto *) {});
    else if (auto *p = dynamic_cast<MidiInputDevice *>(&device))
        midi_input_device_.reset(p, [](auto *) {});
    else if (auto *p = dynamic_cast<MidiOutputDevice *>(&device))
        midi_output_device_.reset(p, [](auto *) {});
}

void Rack::OnRemovingDevice(Device &device) {
    if (audio_input_device_.get() == &device)
        audio_input_device_.reset();
    if (audio_output_device_.get() == &device)
        audio_output_device_.reset();
    if (midi_input_device_.get() == &device)
        midi_input_device_.reset();
    if (midi_output_device_.get() == &device)
        midi_output_device_.reset();
}

// ---------------------------------------------------------------------------
// Device construction
// ---------------------------------------------------------------------------

bool Rack::CreateAudioInputDevice() {
    audio_input_device_ =
        ModelFactoryT<AudioInputDevice>::Make(shared_from_this());
    return true;
}

bool Rack::CreateAudioOutputDevice() {
    audio_output_device_ =
        ModelFactoryT<AudioOutputDevice>::Make(shared_from_this());
    return true;
}

bool Rack::CreateMidiInputDevice() {
    midi_input_device_ =
        ModelFactoryT<MidiInputDevice>::Make(shared_from_this());
    return true;
}

bool Rack::CreateMidiOutputDevice() {
    midi_output_device_ =
        ModelFactoryT<MidiOutputDevice>::Make(shared_from_this());
    return true;
}

void Rack::EnqueueMidiMessage(MidiMessage message) {
    EnqueueAction([this, message]() {
        if (midi_input_device_)
            midi_input_device_->midi_out_->Write(message);
    });
}

} // namespace augr