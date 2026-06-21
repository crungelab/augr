// augr/fm/dexie_pitch_env_module.h
#pragma once
#include <augr/rack/module/module.h>
#include <augr/rack/voltage_pin.h>
#include <cmath>
#include <cstdint>

namespace augr::fm {

class DexiePitchEnvModule : public Module {
public:
    void OnCreate() override {
        Module::OnCreate();
        label_ = "Pitch EG";
    }

    void CreatePins() override {
        gate_in_ = new VoltageInput(*this, "gate");
        AddInput(*gate_in_);
        cv_out_ = new VoltageOutput(*this, "cv_out");
        AddOutput(*cv_out_);
    }

    void CreateControls() override {
        //UiBuilder ui(shared_from_this());
        CreateIntParameter(
            "PR1", ControlMeta::kDefault, &rates_[0], 99, 0, 99);
        CreateIntParameter(
            "PR2", ControlMeta::kDefault, &rates_[1], 99, 0, 99);
        CreateIntParameter(
            "PR3", ControlMeta::kDefault, &rates_[2], 99, 0, 99);
        CreateIntParameter(
            "PR4", ControlMeta::kDefault, &rates_[3], 99, 0, 99);
        CreateIntParameter(
            "PL1", ControlMeta::kDefault, &levels_[0], 50, 0, 99);
        CreateIntParameter(
            "PL2", ControlMeta::kDefault, &levels_[1], 50, 0, 99);
        CreateIntParameter(
            "PL3", ControlMeta::kDefault, &levels_[2], 50, 0, 99);
        CreateIntParameter(
            "PL4", ControlMeta::kDefault, &levels_[3], 50, 0, 99);
        // No knobs needed — these are patch-driven only
    }

    void Process() override {
        const Audio gate_buf = gate_in_->Read();
        const fy_real *gate_data =
            gate_buf.Empty() ? nullptr : gate_buf.array().data();

        // Recompute sr_unit_ if sample rate changed. In practice this is
        // constant after init but cheap to recompute each buffer.
        const float sr = Audio::sample_rate();
        if (sr != last_sr_) {
            last_sr_ = sr;
            sr_unit_ = static_cast<float>(1 << 24) / (21.3f * sr);
        }

        Audio out(ChannelLayout::kMono);
        fy_real *out_data = out.array().data();
        const std::size_t frames = Audio::frames();

        for (std::size_t i = 0; i < frames; ++i) {
            const bool gate = gate_data && (gate_data[i] > 0.5f);
            if (gate && !gate_prev_) {
                down_ = true;
                Advance(0);
            }
            if (!gate && gate_prev_) {
                if (down_) {
                    down_ = false;
                    Advance(3);
                }
            }
            gate_prev_ = gate;
            out_data[i] = static_cast<fy_real>(TickOctaves());
        }

        cv_out_->Write(out);
    }

    void SetRatesLevels(const int rates[4], const int levels[4]) {
        for (int i = 0; i < 4; ++i) {
            rates_[i] = rates[i];
            levels_[i] = levels[i];
        }
        // Start from L4 level, matching Dexed's PitchEnv::set().
        level_ = static_cast<int32_t>(kPitchEnvTab[Clamp99(levels[3])]) << 19;
        down_ = true;
        Advance(0);
    }

    VoltageInput *gate_in_ = nullptr;
    VoltageOutput *cv_out_ = nullptr;

    REFLECT_ENABLE(Module)

private:
    // Pitch level 0..99 -> signed deviation [-128..127].
    // Level 50 = 0 (no deviation). From Dexed's pitchenv_tab.
    static constexpr int8_t kPitchEnvTab[100] = {
        -128, -116, -104, -95, -85, -76, -68, -61, -56, -52, -49, -46, -43,
        -41,  -39,  -37,  -35, -33, -32, -31, -30, -29, -28, -27, -26, -25,
        -24,  -23,  -22,  -21, -20, -19, -18, -17, -16, -15, -14, -13, -12,
        -11,  -10,  -9,   -8,  -7,  -6,  -5,  -4,  -3,  -2,  -1,  0,   1,
        2,    3,    4,    5,   6,   7,   8,   9,   10,  11,  12,  13,  14,
        15,   16,   17,   18,  19,  20,  21,  22,  23,  24,  25,  26,  27,
        28,   29,   30,   31,  32,  33,  34,  35,  38,  40,  43,  46,  49,
        53,   58,   65,   73,  82,  92,  103, 115, 127};

    // Pitch rate 0..99 -> increment multiplier. From Dexed's pitchenv_rate.
    static constexpr uint8_t kPitchEnvRate[100] = {
        1,   2,   3,   3,   4,   4,   5,   5,   6,   6,   7,   7,   8,
        8,   9,   9,   10,  10,  11,  11,  12,  12,  13,  13,  14,  14,
        15,  16,  16,  17,  18,  18,  19,  20,  21,  22,  23,  24,  25,
        26,  27,  28,  30,  31,  33,  34,  36,  37,  38,  39,  41,  42,
        44,  46,  47,  49,  51,  53,  54,  56,  58,  60,  62,  64,  66,
        68,  70,  72,  74,  76,  79,  82,  85,  88,  91,  94,  98,  102,
        106, 110, 115, 120, 125, 130, 135, 141, 147, 153, 159, 165, 171,
        178, 185, 193, 202, 211, 232, 243, 254, 255};

    // --- Envelope state ---
    int rates_[4] = {};
    int levels_[4] = {};
    int32_t level_ = 0;
    int32_t targetlevel_ = 0;
    int32_t inc_ = 0;
    int ix_ = 4;
    bool rising_ = false;
    bool down_ = false;
    bool gate_prev_ = false;
    float sr_unit_ = 1.0f;
    float last_sr_ = 0.0f;

    static int Clamp99(int v) { return v < 0 ? 0 : v > 99 ? 99 : v; }

    void Advance(int newix) {
        ix_ = newix;
        if (ix_ < 4) {
            const int newlevel = Clamp99(levels_[ix_]);
            targetlevel_ = static_cast<int32_t>(kPitchEnvTab[newlevel]) << 19;
            rising_ = targetlevel_ > level_;
            inc_ = static_cast<int32_t>(kPitchEnvRate[Clamp99(rates_[ix_])] *
                                        sr_unit_);
            if (inc_ < 1)
                inc_ = 1;
        }
    }

    float TickOctaves() {
        if (ix_ < 3 || (ix_ < 4 && !down_)) {
            if (rising_) {
                level_ += inc_;
                if (level_ >= targetlevel_) {
                    level_ = targetlevel_;
                    Advance(ix_ + 1);
                }
            } else {
                level_ -= inc_;
                if (level_ <= targetlevel_) {
                    level_ = targetlevel_;
                    Advance(ix_ + 1);
                }
            }
        }
        return static_cast<float>(level_) / static_cast<float>(1 << 24);
    }
};

} // namespace augr::fm