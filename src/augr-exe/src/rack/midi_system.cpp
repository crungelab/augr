#include <spdlog/spdlog.h>

#include <augr/core/midi/midi_message_logger.h>

#include <augr/exe/rack/exe_rack.h>
#include <augr/exe/rack/midi_configurator.h>
#include <augr/exe/rack/midi_system.h>

namespace augr {

MidiSystem::MidiSystem(ExeRack &rack) : rack_(rack) {}

MidiSystem::~MidiSystem() { Stop(); }

void MidiSystem::Callback(double timestamp,
                          std::vector<unsigned char>* raw,
                          void* userdata) {
  if (!raw || raw->empty()) return;

  MidiMessage message = MidiMessage::FromBytes(
      raw->data(),
      static_cast<uint8_t>(raw->size()),
      timestamp);

  auto* data = static_cast<PortCallbackData*>(userdata);
  LogMidiMessage(message, data->port_name);
  data->system->rack_.EnqueueMidiMessage(std::move(message));
}

bool MidiSystem::Create() {
    std::unique_ptr<RtMidiIn> probe;
    try {
        probe =
            std::make_unique<RtMidiIn>(RtMidi::LINUX_ALSA, "augr-midi-probe");
    } catch (RtMidiError &e) {
        spdlog::error("MidiSystem: failed to create RtMidiIn: {}",
                      e.getMessage());
        return false;
    }

    MidiConfig config;
    config.openAllDevicePorts = true;

    MidiConfigurator configurator(*probe);
    if (!configurator.configure(config)) {
        return false;
    }

    for (unsigned int port_index : config.inputPortIndices) {
        std::string name = probe->getPortName(port_index);
        try {
            auto midi_in = std::make_unique<RtMidiIn>(
                RtMidi::LINUX_ALSA, "augr-midi-" + std::to_string(port_index));

            auto cb_data = std::make_unique<PortCallbackData>();
            cb_data->system = this;
            cb_data->port_index = port_index;
            cb_data->port_name = name;

            midi_in->openPort(port_index, name);
            midi_in->ignoreTypes(false, true, true);
            midi_in->setCallback(&MidiSystem::Callback, cb_data.get());

            spdlog::info("MidiSystem: opened port {} \"{}\"", port_index, name);

            inputs_.push_back(std::move(midi_in));
            callback_data_.push_back(std::move(cb_data));

        } catch (RtMidiError &e) {
            spdlog::warn("MidiSystem: failed to open port {} \"{}\": {}",
                         port_index, name, e.getMessage());
        }
    }

    if (inputs_.empty()) {
        spdlog::warn("MidiSystem: no usable MIDI input ports opened");
    }

    return true;
}

void MidiSystem::Stop() {
    for (auto &midi_in : inputs_) {
        if (midi_in->isPortOpen()) {
            midi_in->cancelCallback();
            midi_in->closePort();
        }
    }
    inputs_.clear();
    callback_data_.clear();
}

unsigned int MidiSystem::port_count() const {
    return static_cast<unsigned int>(inputs_.size());
}

} // namespace augr