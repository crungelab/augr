#pragma once

enum class UnpackedOpField : int {
    kRate1 = 0, kRate2 = 1, kRate3 = 2, kRate4 = 3,
    kLevel1 = 4, kLevel2 = 5, kLevel3 = 6, kLevel4 = 7,
    kKbdBreakPoint   = 8,   // plain byte
    kKbdLeftDepth    = 9,   // plain byte
    kKbdRightDepth   = 10,  // plain byte
    kKbdLeftCurve    = 11,  // tmp & 0x03
    kKbdRightCurve   = 12,  // (tmp>>2) & 0x03
    kKbdRateScaling  = 13,  // tmp & 0x07  — NOT currently parsed into Dx7Op
    kAmpModSens      = 14,  // tmp & 0x03
    kVelocitySens    = 15,  // (tmp>>2) & 0x07, already shifted by UnpackVoice
    kOutputLevel     = 16,  // plain byte
    kOscMode         = 17,  // tmp & 0x01
    kCoarse          = 18,  // (tmp>>1) & 0x1F, already shifted by UnpackVoice
    kFine            = 19,  // plain byte
    kDetune          = 20,  // (tmp>>3) & 0x0F, already shifted by UnpackVoice
};
// All 21 fields now verified directly against UnpackVoice. No remaining
// unverified entries in this enum.

constexpr int kOpFieldCount = 21;

enum class UnpackedGlobalField : int {
    kPitchEgRate1 = 0, kPitchEgRate2 = 1, kPitchEgRate3 = 2, kPitchEgRate4 = 3,
    kPitchEgLevel1 = 4, kPitchEgLevel2 = 5, kPitchEgLevel3 = 6, kPitchEgLevel4 = 7,
    kAlgorithm       = 8,   // plain byte
    kFeedback        = 9,   // tmp & 0x07
    kOscKeySync      = 10,  // (tmp>>3) & 0x01, shares byte with feedback — NOT currently parsed
    kLfoSpeed        = 11,  // plain byte
    kLfoDelay        = 12,  // plain byte
    kLfoPitchDepth   = 13,  // plain byte
    kLfoAmpDepth     = 14,  // plain byte
    kLfoSync         = 15,  // tmp & 0x01
    kLfoWaveform     = 16,  // (tmp>>1) & 0x07, already shifted
    kPitchModSens    = 17,  // (tmp>>4) & 0x07, already shifted
    kTranspose       = 18,  // plain byte
    kNameStart       = 19,  // 10 bytes, us[19..28]
};

constexpr int kGlobalFieldCount = 20; // approximate, unverified