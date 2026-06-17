#pragma once

#include <augr/midi/midi_message.h>
#include <spdlog/spdlog.h>

namespace augr {

inline void LogMidiMessage(const MidiMessage &msg,
                           const std::string &port_name = "") {
    const char *port = port_name.empty() ? "" : port_name.c_str();
    const int ch = msg.channel() + 1; // display as 1-16

    if (msg.IsNoteOn()) {
        spdlog::debug(
            "[MIDI{}] NoteOn     ch={:2d} key={:3d} vel={:3d}  t={:.4f}",
            port_name.empty() ? "" : fmt::format(" {}", port), ch, msg.key(),
            msg.velocity(), msg.seconds());

    } else if (msg.IsNoteOff()) {
        spdlog::debug(
            "[MIDI{}] NoteOff    ch={:2d} key={:3d} vel={:3d}  t={:.4f}",
            port_name.empty() ? "" : fmt::format(" {}", port), ch, msg.key(),
            msg.velocity(), msg.seconds());

    } else if (msg.IsController()) {
        spdlog::debug(
            "[MIDI{}] Controller ch={:2d} num={:3d} val={:3d}  t={:.4f}",
            port_name.empty() ? "" : fmt::format(" {}", port), ch,
            msg.controller_number(), msg.controller_value(), msg.seconds());

    } else if (msg.IsPitchBend()) {
        spdlog::debug(
            "[MIDI{}] PitchBend  ch={:2d} val={:6d}           t={:.4f}",
            port_name.empty() ? "" : fmt::format(" {}", port), ch,
            msg.PitchBendValue(), msg.seconds());

    } else if (msg.IsProgramChange()) {
        spdlog::debug(
            "[MIDI{}] ProgChange ch={:2d} pgm={:3d}            t={:.4f}",
            port_name.empty() ? "" : fmt::format(" {}", port), ch, msg.data1(),
            msg.seconds());

    } else if (msg.IsAftertouch()) {
        spdlog::debug(
            "[MIDI{}] Aftertouch ch={:2d} key={:3d} prs={:3d}  t={:.4f}",
            port_name.empty() ? "" : fmt::format(" {}", port), ch, msg.key(),
            msg.data2(), msg.seconds());

    } else if (msg.IsChannelPressure()) {
        spdlog::debug(
            "[MIDI{}] ChanPress  ch={:2d} prs={:3d}            t={:.4f}",
            port_name.empty() ? "" : fmt::format(" {}", port), ch, msg.data1(),
            msg.seconds());

    } else {
        // Unknown or system message — log raw bytes
        spdlog::debug(
            "[MIDI{}] Unknown    {:02X} {:02X} {:02X} ({} bytes)  t={:.4f}",
            port_name.empty() ? "" : fmt::format(" {}", port), msg.byte(0),
            msg.byte(1), msg.byte(2), msg.size(), msg.seconds());
    }
}

} // namespace augr