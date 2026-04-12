#pragma once

#include <array>
#include <cstdint>

namespace augr {

// Fixed-size MIDI message for use on the audio thread. Supports all common
// channel messages (1-3 bytes). SysEx is not handled here; use
// smf::MidiMessage for offline file I/O.
class MidiMessage {
public:
    MidiMessage() = default;

    // Construct from raw bytes. |size| must be 1, 2, or 3.
    MidiMessage(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t size,
                double seconds)
        : bytes_({b0, b1, b2}), size_(size), seconds_(seconds) {}

    // Factory helpers
    static MidiMessage NoteOn(int channel, int key, int velocity,
                              double seconds = 0.0);
    static MidiMessage NoteOff(int channel, int key, int velocity,
                               double seconds = 0.0);
    static MidiMessage Controller(int channel, int number, int value,
                                  double seconds = 0.0);
    static MidiMessage ProgramChange(int channel, int program,
                                     double seconds = 0.0);
    static MidiMessage PitchBend(int channel, int value, double seconds = 0.0);
    static MidiMessage Aftertouch(int channel, int key, int pressure,
                                  double seconds = 0.0);
    static MidiMessage ChannelPressure(int channel, int pressure,
                                       double seconds = 0.0);

    // Construct from a raw RtMidi buffer. |size| must be 1, 2, or 3.
    static MidiMessage FromBytes(const uint8_t *bytes, uint8_t size,
                                 double seconds = 0.0);

    // Raw access
    uint8_t byte(int index) const { return bytes_[index]; }
    uint8_t size() const { return size_; }
    double seconds() const { return seconds_; }
    void set_seconds(double seconds) { seconds_ = seconds; }

    // Status byte components
    uint8_t status() const { return bytes_[0] & 0xF0; }
    uint8_t channel() const { return bytes_[0] & 0x0F; }
    uint8_t data1() const { return bytes_[1]; }
    uint8_t data2() const { return bytes_[2]; }

    // Message type predicates
    bool IsNoteOn() const { return status() == 0x90 && data2() > 0; }
    bool IsNoteOff() const {
        return status() == 0x80 || (status() == 0x90 && data2() == 0);
    }
    bool IsNote() const { return IsNoteOn() || IsNoteOff(); }
    bool IsController() const { return status() == 0xB0; }
    bool IsPitchBend() const { return status() == 0xE0; }
    bool IsProgramChange() const { return status() == 0xC0; }
    bool IsAftertouch() const { return status() == 0xA0; }
    bool IsChannelPressure() const { return status() == 0xD0; }

    // Note accessors
    uint8_t key() const { return data1(); }
    uint8_t velocity() const { return data2(); }

    // Controller accessors
    uint8_t controller_number() const { return data1(); }
    uint8_t controller_value() const { return data2(); }

    // Common controllers
    bool IsSustain() const { return IsController() && data1() == 64; }
    bool IsSustainOn() const { return IsSustain() && data2() >= 64; }
    bool IsSustainOff() const { return IsSustain() && data2() < 64; }
    bool IsSoftPedal() const { return IsController() && data1() == 67; }

    // Pitch bend: 14-bit signed, centre = 0, range = [-8192, 8191]
    int PitchBendValue() const {
        return static_cast<int>((data2() << 7) | data1()) - 8192;
    }

private:
    std::array<uint8_t, 3> bytes_ = {};
    uint8_t size_ = 0;
    double seconds_ = 0.0;
};

} // namespace augr