// augr/fm/dx7_patch.h
#pragma once

#include <array>
#include <cstdint>
#include <span>
#include <string>
#include <vector>

namespace augr::fm {

struct Dx7Op {
    float rates[4]  = { 99.f, 99.f, 99.f, 99.f };
    float levels[4] = { 99.f, 99.f, 99.f,  0.f };
    float output_level  = 99.f;
    float ratio_coarse  = 1.f;
    float ratio_fine    = 0.f;
    float detune        = 0.f;   // -7..7
    bool  fixed_freq    = false;
    int   amp_mod_sens   = 0;     // 0..3, raw DX7 AMS value
};

struct Dx7Patch {
    std::string name;
    int algorithm = 0;   // 0..31
    int feedback  = 0;   // 0..7
    std::array<Dx7Op, 6> ops;  // ops[0] = OP1 .. ops[5] = OP6
};

bool ParseDx7Voice(std::span<const uint8_t> data, Dx7Patch& out);
std::vector<Dx7Patch> ParseDx7Cartridge(std::span<const uint8_t> sysex);

} // namespace augr::fm