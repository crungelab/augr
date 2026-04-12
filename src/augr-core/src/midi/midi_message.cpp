#include <augr/core/midi/midi_message.h>

namespace augr {

// static
MidiMessage MidiMessage::FromBytes(const uint8_t *bytes, uint8_t size,
                                   double seconds) {
    MidiMessage msg;
    msg.size_ = size;
    msg.seconds_ = seconds;
    for (uint8_t i = 0; i < size && i < 3; ++i) {
        msg.bytes_[i] = bytes[i];
    }
    return msg;
}

// static
MidiMessage MidiMessage::NoteOn(int channel, int key, int velocity,
                                double seconds) {
    return MidiMessage(static_cast<uint8_t>(0x90 | (channel & 0x0F)),
                       static_cast<uint8_t>(key & 0x7F),
                       static_cast<uint8_t>(velocity & 0x7F), 3, seconds);
}

// static
MidiMessage MidiMessage::NoteOff(int channel, int key, int velocity,
                                 double seconds) {
    return MidiMessage(static_cast<uint8_t>(0x80 | (channel & 0x0F)),
                       static_cast<uint8_t>(key & 0x7F),
                       static_cast<uint8_t>(velocity & 0x7F), 3, seconds);
}

// static
MidiMessage MidiMessage::Controller(int channel, int number, int value,
                                    double seconds) {
    return MidiMessage(static_cast<uint8_t>(0xB0 | (channel & 0x0F)),
                       static_cast<uint8_t>(number & 0x7F),
                       static_cast<uint8_t>(value & 0x7F), 3, seconds);
}

// static
MidiMessage MidiMessage::ProgramChange(int channel, int program,
                                       double seconds) {
    return MidiMessage(static_cast<uint8_t>(0xC0 | (channel & 0x0F)),
                       static_cast<uint8_t>(program & 0x7F), 0, 2, seconds);
}

// static
MidiMessage MidiMessage::PitchBend(int channel, int value, double seconds) {
    // value is 14-bit signed [-8192, 8191], centre = 0
    const int unsigned_value = value + 8192;
    return MidiMessage(
        static_cast<uint8_t>(0xE0 | (channel & 0x0F)),
        static_cast<uint8_t>(unsigned_value & 0x7F),        // LSB
        static_cast<uint8_t>((unsigned_value >> 7) & 0x7F), // MSB
        3, seconds);
}

// static
MidiMessage MidiMessage::Aftertouch(int channel, int key, int pressure,
                                    double seconds) {
    return MidiMessage(static_cast<uint8_t>(0xA0 | (channel & 0x0F)),
                       static_cast<uint8_t>(key & 0x7F),
                       static_cast<uint8_t>(pressure & 0x7F), 3, seconds);
}

// static
MidiMessage MidiMessage::ChannelPressure(int channel, int pressure,
                                         double seconds) {
    return MidiMessage(static_cast<uint8_t>(0xD0 | (channel & 0x0F)),
                       static_cast<uint8_t>(pressure & 0x7F), 0, 2, seconds);
}

} // namespace augr