#pragma once

#include <memory>
#include <string>
#include <vector>

#include <RtMidi.h>

#include <augr/core/midi/midi_message.h>

namespace augr {

class ExeRack;

class MidiSystem {
public:
    explicit MidiSystem(ExeRack &rack);
    ~MidiSystem();

    bool Configure();
    void Stop();

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
};

} // namespace augr