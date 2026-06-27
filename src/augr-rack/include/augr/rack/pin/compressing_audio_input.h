#pragma once

#include <augr/rack/pin/audio_pin.h>

namespace augr {

// An AudioInput variant for carrier summing — applies headroom scaling
// to reduce clipping when multiple carriers sum into one output.
// Distinct from plain AudioInput/VoltageInput, where summation (e.g.
// FM phase modulation) must remain unscaled.
class CompressingAudioInput : public AudioInput {
public:
    // using AudioInput::AudioInput;
    CompressingAudioInput(Node &node, std::string name,
                          ChannelLayout layout = ChannelLayout::kMono)
        : AudioInput(node, name, layout) {
        compressor_.RecomputeCoeffs();
    }

    Audio Reduce() const override;

    // Data members
    struct Compressor {
        // params
        fy_real threshold_db = -6;
        fy_real ratio = 4;
        fy_real attack_ms = 5;
        fy_real release_ms = 80;
        fy_real makeup_db = 0;
        fy_real sample_rate = 48000;

        // cached coefficients (call RecomputeCoeffs when SR or times change)
        fy_real attack_coeff = 0;
        fy_real release_coeff = 0;

        // state — persists across blocks
        fy_real gr_smooth_db = 0; // current smoothed gain reduction, dB (<= 0)

        void RecomputeCoeffs() {
            const fy_real a = std::max(attack_ms, fy_real(0.01)) *
                              fy_real(1e-3) * sample_rate;
            const fy_real r = std::max(release_ms, fy_real(0.01)) *
                              fy_real(1e-3) * sample_rate;
            attack_coeff = std::exp(fy_real(-1) / a);
            release_coeff = std::exp(fy_real(-1) / r);
        }

        void Process(Audio &audio) {
            auto &a = audio.array();
            const auto shape = a.shape(); // [channels][frames]
            const size_t channels = shape[0];
            const size_t frames = shape[1];
            constexpr fy_real kEps = fy_real(1e-9);
            constexpr fy_real kDbToLin = fy_real(0.11512925465); // ln(10)/20

            for (size_t f = 0; f < frames; ++f) {
                // 1) linked peak detection: one gain for all channels of this
                //    frame, so the stereo image doesn't shift under compression
                fy_real peak = kEps;
                for (size_t c = 0; c < channels; ++c)
                    peak = std::max(peak, std::abs(a(c, f)));

                // 2) static gain computer, hard knee, dB domain
                const fy_real peak_db = fy_real(20) * std::log10(peak);
                const fy_real over = peak_db - threshold_db;
                const fy_real target_gr =
                    (over > 0) ? -over * (fy_real(1) - fy_real(1) / ratio)
                               : fy_real(0);

                // 3) attack when clamping down (gr more negative), release when
                // letting go
                const fy_real coeff =
                    (target_gr < gr_smooth_db) ? attack_coeff : release_coeff;
                gr_smooth_db =
                    coeff * gr_smooth_db + (fy_real(1) - coeff) * target_gr;

                // 4) apply the shared gain. exp(dB * ln10/20) == 10^(dB/20),
                // cheaper than pow
                const fy_real gain =
                    std::exp((gr_smooth_db + makeup_db) * kDbToLin);
                for (size_t c = 0; c < channels; ++c)
                    a(c, f) *= gain;
            }
        }
    } mutable compressor_;
};

} // namespace augr