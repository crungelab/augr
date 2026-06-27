#pragma once

#include <augr/rack/pin/audio_pin.h>

namespace augr {

// An AudioInput variant for carrier summing — applies brick-wall peak
// limiting so the summed output is held at or below a ceiling. Unlike
// CompressingAudioInput, this *guarantees* the ceiling (instant attack),
// at the cost of some transient distortion on hard peaks.
class LimitingAudioInput : public AudioInput {
public:
    // using AudioInput::AudioInput;
    LimitingAudioInput(Node &node, std::string name,
                       ChannelLayout layout = ChannelLayout::kMono)
        : AudioInput(node, name, layout) {
        limiter_.RecomputeCoeffs();
    }

    Audio Reduce() const override;

    // Data members
    struct Limiter {
        // params
        fy_real ceiling_db = -0.1; // output ceiling, never exceeded
        fy_real release_ms = 50;
        fy_real sample_rate = 48000;

        // cached coefficients (call RecomputeCoeffs when SR/release/ceiling
        // change)
        fy_real ceiling_lin = 0;
        fy_real release_coeff = 0;

        // state — persists across blocks
        fy_real gain = 1; // current gain (<= 1)

        void RecomputeCoeffs() {
            const fy_real r = std::max(release_ms, fy_real(0.01)) *
                              fy_real(1e-3) * sample_rate;
            release_coeff = std::exp(fy_real(-1) / r);
            // 10^(dB/20) == exp(dB * ln10/20)
            ceiling_lin = std::exp(ceiling_db * fy_real(0.11512925465));
        }

        void Process(Audio &audio) {
            auto &a = audio.array();
            const auto shape = a.shape(); // [channels][frames]
            const size_t channels = shape[0];
            const size_t frames = shape[1];
            constexpr fy_real kEps = fy_real(1e-9);

            for (size_t f = 0; f < frames; ++f) {
                // 1) linked peak detection: one gain for all channels of this
                //    frame, so the stereo image doesn't shift under limiting
                fy_real peak = kEps;
                for (size_t c = 0; c < channels; ++c)
                    peak = std::max(peak, std::abs(a(c, f)));

                // 2) gain needed to hold this sample at/below the ceiling
                const fy_real target =
                    (peak > ceiling_lin) ? ceiling_lin / peak : fy_real(1);

                // 3) instant attack (snap down — this is what guarantees the
                //    ceiling), single-pole release back toward unity
                if (target < gain)
                    gain = target;
                else
                    gain = release_coeff * gain +
                           (fy_real(1) - release_coeff) * target;

                // 4) apply the shared gain
                for (size_t c = 0; c < channels; ++c)
                    a(c, f) *= gain;
            }
        }
    } mutable limiter_;
};

} // namespace augr