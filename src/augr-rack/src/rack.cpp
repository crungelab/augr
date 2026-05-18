#include <augr/rack/rack.h>

namespace augr {

Rack *Rack::singleton_;

Rack::~Rack() {
    Stop();
}

// ---------------------------------------------------------------------------
// Default device set
// ---------------------------------------------------------------------------

void Rack::CreateDefaultDevices() {
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

} // namespace augr