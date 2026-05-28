#pragma once

#include <vector>

#include "module_widget.h"

namespace augr {

class SpectralModule;

class SpectralWidget : public ModuleWidgetT<SpectralModule> {
public:
    static constexpr std::size_t kFftSize = 2048;
    static constexpr std::size_t kNumBins = kFftSize / 2 + 1;

    explicit SpectralWidget(SpectralModule& model);

    void DrawView() override;

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