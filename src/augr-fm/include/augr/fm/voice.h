// augr/fm/voice.h
#pragma once

#include <array>
#include <memory>

#include <augr/rack/module/module.h>

#include <augr/fm/algorithm.h>
#include <augr/fm/envelope.h>
#include <augr/fm/operator.h>

namespace augr::fm {

// A single polyphony voice: N operators + N envelopes + an algorithm.
// Processing order is a topological sort of the algorithm's DAG, computed
// once in Create(). Per-sample, each operator reads the summed output of
// its modulators into cv_phase_in_.
template <std::size_t N = 6>
class Voice : public Module {
public:
    bool Create(Part &owner) override;
    void Process() override;
    void RebuildProcessOrder();  // public so ApplyPatch can call it

    Algorithm<N>                       algorithm_{};
    std::array<Operator *, N>          operators_{};
    std::array<Envelope *, N>          envelopes_{};

    VoltageInput *cv_pitch_in_ = nullptr;  // note pitch into all operators
    VoltageInput *gate_in_     = nullptr;  // trigger envelopes

    REFLECT_ENABLE(Module)

private:
    std::array<std::uint8_t, N> process_order_{};  // topo-sorted op indices
};

} // namespace augr::fm