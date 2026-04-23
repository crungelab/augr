// augr/fm/algorithm.h
#pragma once

#include <array>
#include <cstdint>

namespace augr::fm {

// An algorithm describes how N operators are routed.
// `modulates[i]` is a bitmask: bit j set means operator i modulates operator j.
// Bit i on itself is feedback. Carriers are operators whose output leaves the
// algorithm (i.e. no outgoing modulation into another op, or explicitly marked).
//
// This matches the DX7 "algorithm" concept but generalizes to any N and any
// DAG topology. Classic DX7 has 32 algorithms for N=6; FM8-style has more.
template <std::size_t N>
struct Algorithm {
    std::array<std::uint32_t, N> modulates{};   // routing bitmask
    std::uint32_t                carriers = 0;  // bitmask of carrier outputs

    constexpr bool Modulates(std::size_t from, std::size_t to) const {
        return (modulates[from] >> to) & 1u;
    }
    constexpr bool IsCarrier(std::size_t i) const {
        return (carriers >> i) & 1u;
    }
    constexpr bool HasFeedback(std::size_t i) const {
        return Modulates(i, i);
    }
};

} // namespace augr::fm