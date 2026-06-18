#pragma once

#include <algorithm>
#include <cmath>

namespace augr::fm {

struct DexieLfo {
    static double kLfoSpeedHz[100];

    static float SpeedToOctaves(int speed_0_99) {
        const int s = std::clamp(speed_0_99, 0, 99);
        return std::log2(static_cast<float>(kLfoSpeedHz[s]));
    }
    /*
    float LfoSpeedToOctaves(int speed_0_99) {
        // DX7 LFO speed 0..99 spans roughly 0.06 Hz .. 47 Hz. Placeholder
        // exponential mapping — needs calibration against Dexed by ear, same
        // as the envelope/feedback constants were.
        const float t = std::clamp(speed_0_99, 0, 99) / 99.0f;
        const float hz = 0.06f * std::pow(47.0f / 0.06f, t);
        return std::log2(hz); // LfoModule::rate_ is octaves above 1 Hz
    }
    */
};

} // namespace augr::fm