#include <algorithm>
#include <augr/fm/dx7_patch.h>
#include <augr/fm/dx7_unpacked_layout.h>
#include <cstring>

namespace augr::fm {

namespace {

// A 32-voice cartridge packs each voice into 128 bytes to save space.
// Unpacking expands it to the 155-byte format. The differences are:
//   - Packed operator fields share bytes (bit fields)
//   - Some fields are stored in combined bytes
constexpr int kUnpackedVoiceSize = 155;
constexpr int kPackedVoiceSize = 128;

void UnpackVoice(const uint8_t *packed, uint8_t *unpacked) {
    for (int op = 0; op < 6; ++op) {
        const uint8_t *ps = packed + op * 17;
        uint8_t *us = unpacked + op * 21;

        us[0] = ps[0];   // EG R1 — plain byte
        us[1] = ps[1];   // EG R2
        us[2] = ps[2];   // EG R3
        us[3] = ps[3];   // EG R4
        us[4] = ps[4];   // EG L1
        us[5] = ps[5];   // EG L2
        us[6] = ps[6];   // EG L3
        us[7] = ps[7];   // EG L4
        us[8] = ps[8];   // kbd break point
        us[9] = ps[9];   // kbd scale left depth
        us[10] = ps[10]; // kbd scale right depth

        uint8_t tmp = ps[11];
        us[11] = tmp & 0x03;        // kbd scale left curve
        us[12] = (tmp >> 2) & 0x03; // kbd scale right curve

        tmp = ps[12];
        us[20] = (tmp >> 3) & 0x0F; // detune  (note: lives here, not byte 16!)
        us[13] = tmp & 0x07;        // kbd rate scaling

        tmp = ps[13];
        us[15] = (tmp >> 2) & 0x07; // key vel sensitivity
        us[14] = tmp & 0x03;        // amp mod sensitivity

        us[16] = ps[14]; // output level — plain byte

        tmp = ps[15];
        us[18] = (tmp >> 1) & 0x1F; // coarse
        us[17] = tmp & 0x01;        // osc mode

        us[19] = ps[16]; // fine — plain byte
    }

    const uint8_t *ps = packed + 102;
    uint8_t *us = unpacked + 126;

    us[0] = ps[0]; // pitch EG R1 — plain byte
    us[1] = ps[1]; // pitch EG R2
    us[2] = ps[2]; // pitch EG R3
    us[3] = ps[3]; // pitch EG R4
    us[4] = ps[4]; // pitch EG L1
    us[5] = ps[5]; // pitch EG L2
    us[6] = ps[6]; // pitch EG L3
    us[7] = ps[7]; // pitch EG L4
    us[8] = ps[8]; // algorithm

    uint8_t tmp = ps[9];
    us[10] = (tmp >> 3) & 0x01; // osc key sync
    us[9] = tmp & 0x07;         // feedback

    us[11] = ps[10]; // lfo speed
    us[12] = ps[11]; // lfo delay
    us[13] = ps[12]; // lfo pitch mod depth
    us[14] = ps[13]; // lfo amp mod depth

    tmp = ps[14];
    us[17] = (tmp >> 4) & 0x07; // pitch mod sensitivity
    us[16] = (tmp >> 1) & 0x07; // lfo waveform
    us[15] = tmp & 0x01;        // lfo sync

    us[18] = ps[15];                   // transpose
    std::memcpy(us + 19, ps + 16, 10); // name
}

// DX7 rates in sysex are 0..99 but NOT the same scale as our internal 0..99.
// The sysex rate is already 0..99; pass through directly.
// Levels likewise are 0..99 in sysex and in our struct.
float SysexRateToParam(uint8_t v) {
    return static_cast<float>(std::min<int>(v, 99));
}
float SysexLevelToParam(uint8_t v) {
    return static_cast<float>(std::min<int>(v, 99));
}

// DX7 coarse ratio encoding:
//   0      → 0.5
//   1..31  → 1..31
float CoarseToRatio(uint8_t coarse) {
    return coarse == 0 ? 0.5f : static_cast<float>(coarse);
}

// DX7 fine ratio: 0..99 → multiplicative fraction 0..~0.99
// The fine value represents (1 + fine/100) - 1 = fine/100 additive on top of
// coarse.
float FineToRatioFine(uint8_t fine) { return static_cast<float>(fine) / 100.f; }

// Detune: stored 0..14 in sysex, 7 = center (0 cents).
float SysexDetuneToParam(uint8_t raw) {
    return static_cast<float>(static_cast<int>(raw) - 7);
}

inline uint8_t OpByte(const uint8_t *op, UnpackedOpField field) {
    return op[static_cast<int>(field)];
}

inline uint8_t GlobalByte(const uint8_t *g, UnpackedGlobalField field) {
    return g[static_cast<int>(field)];
}

void ParseUnpacked(const uint8_t *u, Dx7Patch &out) {
    // Operators are stored OP6..OP1 in the sysex; reverse to OP1..OP6.
    for (int sysex_idx = 0; sysex_idx < 6; ++sysex_idx) {
        const int our_idx = 5 - sysex_idx;
        const uint8_t *op = u + sysex_idx * 21;
        Dx7Op &dst = out.ops[our_idx];

        dst.rates[0] = SysexRateToParam(OpByte(op, UnpackedOpField::kRate1));
        dst.rates[1] = SysexRateToParam(OpByte(op, UnpackedOpField::kRate2));
        dst.rates[2] = SysexRateToParam(OpByte(op, UnpackedOpField::kRate3));
        dst.rates[3] = SysexRateToParam(OpByte(op, UnpackedOpField::kRate4));

        dst.levels[0] = SysexLevelToParam(OpByte(op, UnpackedOpField::kLevel1));
        dst.levels[1] = SysexLevelToParam(OpByte(op, UnpackedOpField::kLevel2));
        dst.levels[2] = SysexLevelToParam(OpByte(op, UnpackedOpField::kLevel3));
        dst.levels[3] = SysexLevelToParam(OpByte(op, UnpackedOpField::kLevel4));

        // All keyboard scaling fields confirmed against UnpackVoice.
        dst.kbd_break_pt = OpByte(op, UnpackedOpField::kKbdBreakPoint);
        dst.kbd_left_depth = OpByte(op, UnpackedOpField::kKbdLeftDepth);
        dst.kbd_right_depth = OpByte(op, UnpackedOpField::kKbdRightDepth);
        dst.kbd_left_curve = OpByte(op, UnpackedOpField::kKbdLeftCurve) & 0x03;
        dst.kbd_right_curve =
            OpByte(op, UnpackedOpField::kKbdRightCurve) & 0x03;

        // kKbdRateScaling (index 13) is parsed but not yet used — stored for
        // when pitch-dependent envelope rate scaling is implemented.
        dst.kbd_rate_scaling =
            OpByte(op, UnpackedOpField::kKbdRateScaling) & 0x07;

        dst.amp_mod_sens = OpByte(op, UnpackedOpField::kAmpModSens) & 0x03;
        dst.velocity_sens = OpByte(op, UnpackedOpField::kVelocitySens) & 0x07;

        dst.output_level =
            SysexLevelToParam(OpByte(op, UnpackedOpField::kOutputLevel));
        dst.fixed_freq = (OpByte(op, UnpackedOpField::kOscMode) & 0x01) != 0;

        dst.detune_raw =
            OpByte(op, UnpackedOpField::kDetune) & 0x0F; // before centering
        dst.detune = SysexDetuneToParam(dst.detune_raw); // centered -7..7

        dst.coarse_raw = OpByte(op, UnpackedOpField::kCoarse) & 0x1F;
        dst.fine_raw = OpByte(op, UnpackedOpField::kFine) & 0x7F;
        dst.ratio_coarse = CoarseToRatio(dst.coarse_raw);
        dst.ratio_fine = FineToRatioFine(dst.fine_raw);
    }

    // Voice globals, relative to (u + 126). All positions confirmed against
    // UnpackVoice.
    const uint8_t *g = u + 126;

    out.algorithm = GlobalByte(g, UnpackedGlobalField::kAlgorithm) & 0x1F;
    out.feedback = GlobalByte(g, UnpackedGlobalField::kFeedback) & 0x07;

    // kOscKeySync
    out.osc_key_sync = GlobalByte(g, UnpackedGlobalField::kOscKeySync) & 0x01;

    out.lfo_speed = GlobalByte(g, UnpackedGlobalField::kLfoSpeed);
    out.lfo_delay = GlobalByte(g, UnpackedGlobalField::kLfoDelay);
    out.lfo_pitch_depth = GlobalByte(g, UnpackedGlobalField::kLfoPitchDepth);
    out.lfo_amp_depth = GlobalByte(g, UnpackedGlobalField::kLfoAmpDepth);

    // Sync, waveform and pitch mod sensitivity are already shifted/masked by
    // UnpackVoice — no further shift needed here.
    out.lfo_sync = GlobalByte(g, UnpackedGlobalField::kLfoSync) & 0x01;
    out.lfo_waveform = GlobalByte(g, UnpackedGlobalField::kLfoWaveform) & 0x07;
    out.pitch_mod_sens =
        GlobalByte(g, UnpackedGlobalField::kPitchModSens) & 0x07;

    out.transpose = GlobalByte(g, UnpackedGlobalField::kTranspose);

    // Pitch EG — parsed but not yet consumed by Dexie.
    for (int i = 0; i < 4; ++i) {
        out.pitch_eg_rates[i] = GlobalByte(
            g, static_cast<UnpackedGlobalField>(
                   static_cast<int>(UnpackedGlobalField::kPitchEgRate1) + i));
        out.pitch_eg_levels[i] = GlobalByte(
            g, static_cast<UnpackedGlobalField>(
                   static_cast<int>(UnpackedGlobalField::kPitchEgLevel1) + i));
    }

    // Name: 10 bytes starting at kNameStart.
    const char *name_ptr = reinterpret_cast<const char *>(
        g + static_cast<int>(UnpackedGlobalField::kNameStart));
    out.name = std::string(name_ptr, 10);
    auto end = out.name.find_last_not_of(' ');
    out.name = (end != std::string::npos) ? out.name.substr(0, end + 1) : "";
}

} // namespace

bool ParseDx7Voice(std::span<const uint8_t> data, Dx7Patch &out) {
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
    constexpr int kHeaderSize = 6;
    constexpr int kFooterSize = 2;
    constexpr int kCartridgeData = kPackedVoiceSize * 32;
    constexpr int kMinSize = kHeaderSize + kCartridgeData + kFooterSize;

    std::vector<Dx7Patch> result;

    if (static_cast<int>(sysex.size()) < kMinSize)
        return result;

    // Basic sysex header validation
    if (sysex[0] != 0xF0 || sysex[1] != 0x43 || sysex[3] != 0x09)
        return result;

    const uint8_t *data = sysex.data() + kHeaderSize;

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