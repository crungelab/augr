// augr/fm/patch.h
#pragma once

#include <array>
#include <string>

#include <augr/fm/algorithm.h>

namespace augr::fm {

// Serializable patch data — the "sound" independent of voice instances.
// This is what you save/load, what a preset browser lists, what MIDI
// program change selects. Keeping it a plain struct (no Module/Node
// machinery) makes serialization trivial.
template <std::size_t N = 6> struct Patch {
    struct OpParams {
        float ratio = 1.0f;
        float frequency = 0.0f;
        float output_level = 1.0f;
        float feedback = 0.0f;
        float rates[4] = {99.f, 50.f, 50.f, 50.f};
        float levels[4] = {99.f, 75.f, 50.f, 0.f};
    };

    std::string name;
    Algorithm<N> algorithm;
    std::array<OpParams, N> operators;
};

template <std::size_t N> class Voice; // fwd decl

template <std::size_t N>
void ApplyPatch(const Patch<N> &patch, Voice<N> &voice);

template <std::size_t N>
Patch<N> CapturePatch(const Voice<N> &voice, std::string name = {});

} // namespace augr::fm