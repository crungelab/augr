#include "spectral_widget.h"

#include <algorithm>
#include <cmath>
#include <vector>

#include <implot.h>
#include <kiss_fftr.h>

#include <augr/rack/library/spectral_module.h>

namespace augr {

SpectralWidget::SpectralWidget(SpectralModule& model)
    : ModuleWidgetT<SpectralModule>(model) {
    ComputeWindow();
    smoothed_db_.assign(kNumBins, -200.f);
    freqs_.assign(kNumBins, 0.f);
}

void SpectralWidget::ComputeWindow() {
    window_.resize(kFftSize);
    // Hann window: 0.5 * (1 - cos(2πn / (N-1)))
    const float N = static_cast<float>(kFftSize - 1);
    for (std::size_t n = 0; n < kFftSize; ++n) {
        window_[n] = 0.5f * (1.f -
            std::cos(2.f * 3.14159265358979f *
                     static_cast<float>(n) / N));
    }
}

void SpectralWidget::DrawDockContent() {
    // Rebuild bin frequency table if sample rate changed.
    const float sr = model_->SampleRate();
    if (sr != cached_sample_rate_) {
        cached_sample_rate_ = sr;
        for (std::size_t i = 0; i < kNumBins; ++i) {
            freqs_[i] = static_cast<float>(i) * sr /
                        static_cast<float>(kFftSize);
        }
    }

    // Snapshot the most recent kFftSize samples.
    static thread_local std::vector<float> capture;
    capture.resize(kFftSize);
    const std::size_t n = model_->Snapshot(capture.data(), capture.size());

    if (n >= kFftSize) {
        // Apply Hann window and convert to kiss_fft_scalar.
        static thread_local std::vector<float> windowed;
        windowed.resize(kFftSize);
        for (std::size_t i = 0; i < kFftSize; ++i) {
            windowed[i] = capture[i] * window_[i];
        }

        // Run FFT. kiss_fftr does real->complex, output is kNumBins entries.
        static kiss_fftr_cfg cfg =
            kiss_fftr_alloc(static_cast<int>(kFftSize), 0, nullptr, nullptr);
        static thread_local std::vector<kiss_fft_cpx> spectrum(kNumBins);
        kiss_fftr(cfg, windowed.data(), spectrum.data());

        // Convert to dB with smoothing. Normalize by kFftSize/2 so a full-
        // scale sine reads close to 0 dB. Add tiny epsilon to avoid log(0).
        const float norm = 2.f / static_cast<float>(kFftSize);
        const float alpha = std::clamp(model_->smoothing_, 0.f, 0.99f);
        for (std::size_t i = 0; i < kNumBins; ++i) {
            const float re = spectrum[i].r;
            const float im = spectrum[i].i;
            const float mag = std::sqrt(re * re + im * im) * norm;
            const float db = 20.f * std::log10(mag + 1e-9f);
            smoothed_db_[i] = alpha * smoothed_db_[i] + (1.f - alpha) * db;
        }
    }

    // Plot. Log-frequency x-axis is the standard for audio spectrum displays.
    const float floor_db = model_->floor_db_;
    const float ceil_db = 6.f;

    if (ImPlot::BeginPlot("##spectral", ImVec2(-1, 200),
                          ImPlotFlags_CanvasOnly | ImPlotFlags_NoMouseText)) {
        ImPlot::SetupAxes(nullptr, nullptr,
                          ImPlotAxisFlags_NoMenus |
                              ImPlotAxisFlags_NoHighlight,
                          ImPlotAxisFlags_NoMenus |
                              ImPlotAxisFlags_NoHighlight |
                              ImPlotAxisFlags_Lock);
        ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Log10);
        ImPlot::SetupAxisLimits(ImAxis_X1, 20.0,
                                static_cast<double>(cached_sample_rate_ * 0.5),
                                ImPlotCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1,
                                static_cast<double>(floor_db),
                                static_cast<double>(ceil_db),
                                ImPlotCond_Always);

        // Skip bin 0 (DC) for log scale — log(0) is undefined.
        ImPlot::PlotLine("##mag",
                         freqs_.data() + 1,
                         smoothed_db_.data() + 1,
                         static_cast<int>(kNumBins - 1));

        ImPlot::EndPlot();
    }
}

DEFINE_MODEL_WIDGET_FACTORY(SpectralWidget, SpectralModule)

} // namespace augr