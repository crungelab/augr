#pragma once

#include <augr/rack/library/spectral_module.h>

#include "console_view.h"

namespace augr {

class SpectralView : public ViewT<SpectralModule, ConsoleView> {
public:
    void Draw() override;

public:
    static constexpr std::size_t kFftSize = 2048;
    static constexpr std::size_t kNumBins = kFftSize / 2 + 1;

    explicit SpectralView(SpectralModule& model);

private:
    void ComputeWindow();

    // Hann window, precomputed once.
    std::vector<float> window_;

    // Persistent smoothed magnitudes in dB, one per FFT bin.
    std::vector<float> smoothed_db_;

    // Frequency coordinates for each bin (Hz), precomputed per sample rate.
    std::vector<float> freqs_;
    float cached_sample_rate_ = 0.f;
};

} // namespace augr