#pragma once

#include <RtMidi.h>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <cctype>
#include <limits>
#include <optional>
#include <string>
#include <vector>

namespace augr {

struct MidiConfig {
    // Ports to open after configuration.
    // MidiConfigurator populates this; MidiSystem consumes it.
    std::vector<unsigned int> inputPortIndices;

    // If true, open all non-virtual ports rather than just the best one.
    bool openAllDevicePorts = true;
};

class MidiConfigurator {
public:
    explicit MidiConfigurator(RtMidiIn &midi_in) : midi_in_(midi_in) {}

    bool configure(MidiConfig &config) {
        const unsigned int port_count = midi_in_.getPortCount();
        spdlog::debug("MidiConfigurator: port count={}", port_count);

        if (port_count == 0) {
            spdlog::warn("MidiConfigurator: no MIDI input ports found");
            return true; // non-fatal
        }

        // Collect and score all ports
        struct Candidate {
            unsigned int index;
            std::string  name;
            int          score;
        };

        std::vector<Candidate> candidates;
        candidates.reserve(port_count);

        for (unsigned int i = 0; i < port_count; ++i) {
            std::string name = midi_in_.getPortName(i);
            int score = scorePort(name);
            spdlog::debug("MidiConfigurator: port {} \"{}\" score={}", i, name, score);
            candidates.push_back({i, name, score});
        }

        if (config.openAllDevicePorts) {
            // Open every port with a non-negative score (i.e. skip virtual/loopback)
            for (const auto &c : candidates) {
                if (c.score >= 0) {
                    spdlog::info("MidiConfigurator: selecting port {} \"{}\" score={}",
                                 c.index, c.name, c.score);
                    config.inputPortIndices.push_back(c.index);
                } else {
                    spdlog::info("MidiConfigurator: skipping port {} \"{}\" score={}",
                                 c.index, c.name, c.score);
                }
            }

            if (config.inputPortIndices.empty()) {
                spdlog::warn("MidiConfigurator: no usable MIDI input ports after filtering");
            }

            return true;
        }

        // Single best port
        auto best = std::max_element(candidates.begin(), candidates.end(),
                                     [](const Candidate &a, const Candidate &b) {
                                         return a.score < b.score;
                                     });

        if (best == candidates.end() || best->score < 0) {
            spdlog::warn("MidiConfigurator: no usable MIDI input port found");
            return true; // non-fatal
        }

        spdlog::info("MidiConfigurator: selected port {} \"{}\" score={}",
                     best->index, best->name, best->score);
        config.inputPortIndices.push_back(best->index);
        return true;
    }

private:
    RtMidiIn &midi_in_;

    static std::string toLower(std::string s) {
        std::transform(s.begin(), s.end(), s.begin(),
                       [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        return s;
    }

    static bool contains(const std::string &haystack, const std::string &needle) {
        return haystack.find(needle) != std::string::npos;
    }

    static int scorePort(const std::string &name) {
        const std::string lower = toLower(name);
        int score = 0;

        // --- Virtual / loopback ports: strongly negative so they're never
        //     opened in openAllDevicePorts mode and never chosen as best ---
        if (contains(lower, "midi through"))  return -1000;
        if (contains(lower, "through port"))  return -1000;
        if (contains(lower, "virtual"))       return  -500;

        // --- DAW / transport ports: functional but lower priority than the
        //     main instrument port ---
        if (contains(lower, "daw port"))      score -= 30;
        if (contains(lower, "transport"))     score -= 30;

        // --- DIN / hardware thru ports: real but secondary ---
        if (contains(lower, "din port"))      score -= 10;

        // --- Primary instrument ports: these are what you actually play ---
        if (contains(lower, "midi port"))     score += 50;
        if (contains(lower, "usb"))           score += 20;
        if (contains(lower, "keyboard"))      score += 20;
        if (contains(lower, "keys"))          score += 10;

        // --- Known controller brands ---
        if (contains(lower, "akai"))          score += 15;
        if (contains(lower, "mpk"))           score += 15;
        if (contains(lower, "arturia"))       score += 15;
        if (contains(lower, "novation"))      score += 15;
        if (contains(lower, "korg"))          score += 15;
        if (contains(lower, "roland"))        score += 15;
        if (contains(lower, "native instruments")) score += 15;
        if (contains(lower, "focusrite"))     score += 15;
        if (contains(lower, "moog"))          score += 15;
        if (contains(lower, "elektron"))      score += 15;

        return score;
    }
};

} // namespace augr