// augr/fm/patch.cpp
#include <augr/fm/patch.h>

#include <augr/fm/voice.h>

namespace augr::fm {

// Apply a Patch to a live Voice. This is intentionally a free function rather
// than a method on either side — Patch stays a plain serializable struct, and
// Voice doesn't need to know the Patch type exists at its header level.
template <std::size_t N>
void ApplyPatch(const Patch<N> &patch, Voice<N> &voice) {
    voice.algorithm_ = patch.algorithm;

    for (std::size_t i = 0; i < N; ++i) {
        const auto &p = patch.operators[i];
        auto *op = voice.operators_[i];
        auto *env = voice.envelopes_[i];
        if (!op || !env) continue;

        op->ratio_        = p.ratio;
        op->frequency_    = p.frequency;
        op->output_level_ = p.output_level;
        op->feedback_     = p.feedback;

        for (int r = 0; r < 4; ++r) {
            env->rates_[r]  = p.rates[r];
            env->levels_[r] = p.levels[r];
        }
    }

    // Re-sort the processing order in case the algorithm changed.
    // Voice::Create() did this originally; exposing a Rebuild hook avoids
    // a full teardown when swapping patches at runtime.
    voice.RebuildProcessOrder();
}

// Capture a Voice's current state into a Patch. Useful for "save current sound"
// in a UI, or for round-tripping edits back to disk.
template <std::size_t N>
Patch<N> CapturePatch(const Voice<N> &voice, std::string name) {
    Patch<N> patch;
    patch.name      = std::move(name);
    patch.algorithm = voice.algorithm_;

    for (std::size_t i = 0; i < N; ++i) {
        auto &p = patch.operators[i];
        const auto *op = voice.operators_[i];
        const auto *env = voice.envelopes_[i];
        if (!op || !env) continue;

        p.ratio        = op->ratio_;
        p.frequency    = op->frequency_;
        p.output_level = op->output_level_;
        p.feedback     = op->feedback_;

        for (int r = 0; r < 4; ++r) {
            p.rates[r]  = env->rates_[r];
            p.levels[r] = env->levels_[r];
        }
    }
    return patch;
}

// Explicit instantiations matching Voice.
template void       ApplyPatch<2>(const Patch<2> &, Voice<2> &);
template void       ApplyPatch<4>(const Patch<4> &, Voice<4> &);
template void       ApplyPatch<6>(const Patch<6> &, Voice<6> &);
template Patch<2>   CapturePatch<2>(const Voice<2> &, std::string);
template Patch<4>   CapturePatch<4>(const Voice<4> &, std::string);
template Patch<6>   CapturePatch<6>(const Voice<6> &, std::string);

} // namespace augr::fm