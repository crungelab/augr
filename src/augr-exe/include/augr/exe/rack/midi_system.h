#pragma once

#include <memory>
#include <string>
#include <vector>

#include <RtMidi.h>

#include <augr/core/midi/midi_message.h>
#include <augr/exe/rack/midi_configurator.h>

namespace augr {

class ExeRack;

class MidiSystem {
public:
    explicit MidiSystem(ExeRack &rack);
    ~MidiSystem();

    bool Configure(MidiConfig &config);
    bool Start();
    void Stop();
    bool IsRunning() const;

    unsigned int port_count() const;

private:
    struct PortCallbackData {
        MidiSystem *system;
        unsigned int port_index;
        std::string port_name;
    };

    static void Callback(double timestamp, std::vector<unsigned char> *message,
                         void *userdata);

    ExeRack &rack_;
    std::vector<std::unique_ptr<RtMidiIn>> inputs_;
    std::vector<std::unique_ptr<PortCallbackData>> callback_data_;

    MidiConfig config_{};
    std::vector<std::string> port_names_;
    bool configured_ = false;
};

} // namespace augr