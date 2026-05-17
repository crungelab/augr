#include <queue>
#include <unordered_set>

#include <augr/core/model_factory.h>

#include <augr/rack/module/module.h>
#include <augr/rack/rack.h>
#include <augr/rack/wire.h>

#include <augr/rack/module/audio_device.h>
#include <augr/rack/module/midi_device.h>

namespace augr {

Rack *Rack::singleton_;

Rack::~Rack() {
    Stop();
}

void Rack::CreateDefaultDevices() {
    if (config_.audio_input_channels > 0)
        CreateAudioInputDevice();
    CreateAudioOutputDevice();
    if (config_.enable_midi_input)
        CreateMidiInputDevice();
    if (config_.enable_midi_output)
        CreateMidiOutputDevice();
}


/*
void Rack::OnAddingChild(Model &model) {
    Graph::OnAddingChild(model);

    if (dynamic_cast<Device *>(&model)) {
        // Devices are owned by the rack but not added to modules_.
        // The rack drives them directly via the audio callback.
        //graph_dirty_ = true;
        return;
    }

    if (auto *m = dynamic_cast<Module *>(&model)) {
        AddModule(*m);
        graph_dirty_ = true;
    }
}
*/

/*
void Rack::OnAddingChild(Model &model) {
    Graph::OnAddingChild(model);
    if (auto *m = dynamic_cast<Module *>(&model)) {
        EnqueueAction([this, m]{
            AddModule(*m);
            graph_dirty_ = true;
        });
    }
}
*/

void Rack::OnAddingChild(Model &model) {
    Graph::OnAddingChild(model);

    if (auto *m = dynamic_cast<Module *>(&model)) {
        AddModule(*m);
        graph_dirty_ = true;
    }
    if (auto *d = dynamic_cast<Device *>(&model)) {
        OnAddingDevice(*d);
    }
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

void Rack::OnRemovingChild(Model &model) {
    if (auto *d = dynamic_cast<Device *>(&model)) {
        OnRemovingDevice(*d);
    }
    if (auto *m = dynamic_cast<Module *>(&model)) {
        RemoveModule(*m);
        graph_dirty_ = true;
    }
    Graph::OnRemovingChild(model);
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

// ---------------------------------------------------------------------------
// MIDI thread entry point — just enqueue; processed on the audio thread
// ---------------------------------------------------------------------------

void Rack::EnqueueMidiMessage(MidiMessage message) {
    EnqueueAction([this, message = std::move(message)]() {
        midi_input_device_->midi_out_->Write(message);
    });
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

void Rack::RebuildExecutionOrder() {
    std::unordered_map<Module *, std::vector<Module *>> dependents;
    std::unordered_map<Module *, int> in_degree;

    // Initialize all modules
    for (Module *m : modules_) {
        in_degree[m] = 0;
    }

    // Build graph: m -> downstream
    // Deduplicate edges so multiple wires between the same two modules
    // do not inflate in_degree.
    for (Module *m : modules_) {
        std::unordered_set<Module *> seen_downstream;

        for (auto *out_pin : m->outport_.pins_) {
            for (auto *out_wire : out_pin->wires_) {
                if (!out_wire || !out_wire->input_ ||
                    !out_wire->input_->node_) {
                    continue;
                }

                Module *downstream =
                    dynamic_cast<Module *>(out_wire->input_->node_);
                if (!downstream || downstream == m) {
                    continue;
                }

                if (seen_downstream.insert(downstream).second) {
                    dependents[m].push_back(downstream);
                    ++in_degree[downstream];
                }
            }
        }
    }

    // Seed with every module that has no upstream dependencies.
    std::queue<Module *> ready;
    for (Module *m : modules_) {
        if (in_degree[m] == 0) {
            ready.push(m);
        }
    }

    sorted_modules_.clear();
    sorted_modules_.reserve(modules_.size());

    while (!ready.empty()) {
        Module *m = ready.front();
        ready.pop();

        sorted_modules_.push_back(m);

        auto it = dependents.find(m);
        if (it == dependents.end()) {
            continue;
        }

        for (Module *dep : it->second) {
            if (--in_degree[dep] == 0) {
                ready.push(dep);
            }
        }
    }

    // Cycle handling:
    // If anything remains, those modules are part of a cycle
    // or are otherwise unreachable by Kahn's algorithm.
    if (sorted_modules_.size() != modules_.size()) {
        std::unordered_set<Module *> already_sorted(sorted_modules_.begin(),
                                                    sorted_modules_.end());

        for (Module *m : modules_) {
            if (!already_sorted.contains(m)) {
                sorted_modules_.push_back(m);
            }
        }

        // TODO: log/report cycle detected here
    }

    graph_dirty_ = false;
}

// ---------------------------------------------------------------------------
// Device module helpers
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

} // namespace augr