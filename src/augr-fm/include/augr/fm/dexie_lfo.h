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
};

} // namespace augr::fm