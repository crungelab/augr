// augr/fm/dx7_patch.cpp
#include <augr/fm/dx7_patch.h>

#include <algorithm>
#include <cstring>

namespace augr::fm {

namespace {

// A 32-voice cartridge packs each voice into 128 bytes to save space.
// Unpacking expands it to the 155-byte format. The differences are:
//   - Packed operator fields share bytes (bit fields)
//   - Some fields are stored in combined bytes
constexpr int kUnpackedVoiceSize = 155;
constexpr int kPackedVoiceSize   = 128;

// Unpack a single 128-byte packed voice into a 155-byte unpacked buffer.
void UnpackVoice(const uint8_t* packed, uint8_t* unpacked) {
    // Each operator: 17 packed bytes → 21 unpacked bytes
    for (int op = 0; op < 6; ++op) {
        const uint8_t* ps = packed   + op * 17;
        uint8_t*       us = unpacked + op * 21;

        us[0]  = ps[0] & 0x0F;          // EG R1
        us[1]  = ps[1] & 0x0F;          // EG R2
        us[2]  = ps[2] & 0x0F;          // EG R3
        us[3]  = ps[3] & 0x0F;          // EG R4
        us[4]  = ps[4] & 0x0F;          // EG L1
        us[5]  = ps[5] & 0x0F;          // EG L2
        us[6]  = ps[6] & 0x0F;          // EG L3
        us[7]  = ps[7] & 0x0F;          // EG L4
        us[8]  = ps[8] & 0x1F;          // kbd break point
        us[9]  = ps[9] & 0x1F;          // kbd scale left depth
        us[10] = ps[10] & 0x1F;         // kbd scale right depth
        us[11] = ps[11] & 0x03;         // kbd scale left curve
        us[12] = (ps[11] >> 2) & 0x03;  // kbd scale right curve
        us[13] = ps[12] & 0x07;         // kbd rate scaling
        us[14] = ps[13] & 0x03;         // amp mod sensitivity
        us[15] = (ps[13] >> 2) & 0x07;  // key vel sensitivity
        us[16] = ps[14] & 0x7F;         // output level
        us[17] = ps[15] & 0x01;         // osc mode (0=ratio, 1=fixed)
        us[18] = (ps[15] >> 1) & 0x1F;  // coarse ratio / fixed freq coarse
        us[19] = ps[16] & 0x0F;         // fine ratio / fixed freq fine (0..99)
        us[20] = (ps[16] >> 4) & 0x0F;  // detune (0..14, center=7)
    }

    // Remaining voice parameters (after 6 operators)
    const uint8_t* ps = packed   + 102;
    uint8_t*       us = unpacked + 126;

    us[0]  = ps[0] & 0x0F;   // pitch EG R1
    us[1]  = ps[1] & 0x0F;   // pitch EG R2
    us[2]  = ps[2] & 0x0F;   // pitch EG R3
    us[3]  = ps[3] & 0x0F;   // pitch EG R4
    us[4]  = ps[4] & 0x0F;   // pitch EG L1
    us[5]  = ps[5] & 0x0F;   // pitch EG L2
    us[6]  = ps[6] & 0x0F;   // pitch EG L3
    us[7]  = ps[7] & 0x0F;   // pitch EG L4
    us[8]  = ps[8] & 0x1F;   // algorithm (0..31)
    us[9]  = ps[9] & 0x07;   // feedback
    us[10] = (ps[9] >> 3) & 0x01;  // osc key sync
    std::memcpy(us + 11, ps + 10, 4);  // LFO speed, delay, pitch mod depth, amp mod depth
    us[15] = ps[14] & 0x01;  // LFO sync
    us[16] = (ps[14] >> 1) & 0x07;  // LFO waveform
    us[17] = (ps[14] >> 4) & 0x07;  // pitch mod sensitivity
    us[18] = ps[15] & 0x7F;  // transpose
    std::memcpy(us + 19, ps + 16, 10); // voice name
    // remaining 6 bytes are padding / operator on/off
}

// DX7 rates in sysex are 0..99 but NOT the same scale as our internal 0..99.
// The sysex rate is already 0..99; pass through directly.
// Levels likewise are 0..99 in sysex and in our struct.
float SysexRateToParam(uint8_t v)  { return static_cast<float>(std::min<int>(v, 99)); }
float SysexLevelToParam(uint8_t v) { return static_cast<float>(std::min<int>(v, 99)); }

// DX7 coarse ratio encoding:
//   0      → 0.5
//   1..31  → 1..31
float CoarseToRatio(uint8_t coarse) {
    return coarse == 0 ? 0.5f : static_cast<float>(coarse);
}

// DX7 fine ratio: 0..99 → multiplicative fraction 0..~0.99
// The fine value represents (1 + fine/100) - 1 = fine/100 additive on top of coarse.
float FineToRatioFine(uint8_t fine) {
    return static_cast<float>(fine) / 100.f;
}

// Detune: stored 0..14 in sysex, 7 = center (0 cents).
float SysexDetuneToParam(uint8_t raw) {
    return static_cast<float>(static_cast<int>(raw) - 7);
}

void ParseUnpacked(const uint8_t* u, Dx7Patch& out) {
    // Operators are stored OP6..OP1 in the sysex; reverse to OP1..OP6.
    for (int sysex_idx = 0; sysex_idx < 6; ++sysex_idx) {
        const int our_idx  = 5 - sysex_idx;
        const uint8_t* op  = u + sysex_idx * 21;
        Dx7Op& dst         = out.ops[our_idx];

        dst.rates[0] = SysexRateToParam(op[0]);
        dst.rates[1] = SysexRateToParam(op[1]);
        dst.rates[2] = SysexRateToParam(op[2]);
        dst.rates[3] = SysexRateToParam(op[3]);

        dst.levels[0] = SysexLevelToParam(op[4]);
        dst.levels[1] = SysexLevelToParam(op[5]);
        dst.levels[2] = SysexLevelToParam(op[6]);
        dst.levels[3] = SysexLevelToParam(op[7]);

        // op[8..15]: keyboard scaling and velocity — skip for now
        dst.output_level = SysexLevelToParam(op[16]);
        dst.fixed_freq   = (op[17] & 0x01) != 0;
        dst.ratio_coarse = CoarseToRatio(op[18] & 0x1F);
        dst.ratio_fine   = FineToRatioFine(op[19] & 0x7F);
        dst.detune       = SysexDetuneToParam(op[20] & 0x0F);
    }

    // Voice globals start at byte 126
    const uint8_t* g = u + 126;
    out.algorithm = (g[8] & 0x1F);  // keep 0-based (sysex is also 0-based here after unpack)
    out.feedback  = (g[9] & 0x07);

    // Name is at byte 145 (126 + 19)
    const char* name_ptr = reinterpret_cast<const char*>(u + 145);
    out.name = std::string(name_ptr, 10);
    // Trim trailing spaces
    auto end = out.name.find_last_not_of(' ');
    out.name = (end != std::string::npos) ? out.name.substr(0, end + 1) : "";
}

} // namespace

bool ParseDx7Voice(std::span<const uint8_t> data, Dx7Patch& out) {
    if (data.size() < kUnpackedVoiceSize)
        return false;
    ParseUnpacked(data.data(), out);
    return true;
}

std::vector<Dx7Patch> ParseDx7Cartridge(std::span<const uint8_t> sysex) {
    // A cartridge sysex is:
    //   F0 43 0n 09 20 00  (6 bytes header, n = device number)
    //   128 * 32 = 4096 bytes of packed voice data
    //   checksum (1 byte)
    //   F7 (1 byte)
    // Total: 4104 bytes minimum.
    constexpr int kHeaderSize    = 6;
    constexpr int kFooterSize    = 2;
    constexpr int kCartridgeData = kPackedVoiceSize * 32;
    constexpr int kMinSize       = kHeaderSize + kCartridgeData + kFooterSize;

    std::vector<Dx7Patch> result;

    if (static_cast<int>(sysex.size()) < kMinSize)
        return result;

    // Basic sysex header validation
    if (sysex[0] != 0xF0 || sysex[1] != 0x43 || sysex[3] != 0x09)
        return result;

    const uint8_t* data = sysex.data() + kHeaderSize;

    uint8_t unpacked[kUnpackedVoiceSize];
    for (int v = 0; v < 32; ++v) {
        std::memset(unpacked, 0, sizeof(unpacked));
        UnpackVoice(data + v * kPackedVoiceSize, unpacked);
        Dx7Patch patch;
        ParseUnpacked(unpacked, patch);
        result.push_back(std::move(patch));
    }

    return result;
}

} // namespace augr::fm