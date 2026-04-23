// augr/fm/voice.cpp

#include <algorithm>
#include <array>
#include <cstring>

#include <augr/fm/voice.h>

namespace augr::fm {

namespace {

// Kahn's algorithm for topological sort of the operator DAG.
// Returns true if the graph is acyclic (ignoring self-loops, which are
// feedback).
template <std::size_t N>
bool TopoSort(const Algorithm<N> &algo, std::array<std::uint8_t, N> &out) {
    std::array<int, N> indegree{};
    for (std::size_t i = 0; i < N; ++i) {
        for (std::size_t j = 0; j < N; ++j) {
            if (i != j && algo.Modulates(i, j))
                ++indegree[j];
        }
    }

    std::array<std::uint8_t, N> queue{};
    std::size_t qhead = 0, qtail = 0, written = 0;
    for (std::size_t i = 0; i < N; ++i) {
        if (indegree[i] == 0)
            queue[qtail++] = static_cast<std::uint8_t>(i);
    }
    while (qhead < qtail) {
        const std::uint8_t u = queue[qhead++];
        out[written++] = u;
        for (std::size_t v = 0; v < N; ++v) {
            if (u != v && algo.Modulates(u, v)) {
                if (--indegree[v] == 0)
                    queue[qtail++] = static_cast<std::uint8_t>(v);
            }
        }
    }
    return written == N;
}

} // namespace

template <std::size_t N> bool Voice<N>::Create(Part &owner) {
    if (!Module::Create(owner))
        return false;

    cv_pitch_in_ = new VoltageInput(*this, "pitch");
    AddInput(*cv_pitch_in_);
    gate_in_ = new VoltageInput(*this, "gate");
    AddInput(*gate_in_);
    audio_out_ = new AudioOutput(*this, "out", ChannelLayout::kMono);
    AddOutput(*audio_out_);

    for (std::size_t i = 0; i < N; ++i) {
        operators_[i] = new Operator();
        AddChild(*operators_[i]);
        envelopes_[i] = new Envelope();
        AddChild(*envelopes_[i]);
        operators_[i]->Create(owner);
        envelopes_[i]->Create(owner);
    }

    if (!TopoSort(algorithm_, process_order_)) {
        for (std::size_t i = 0; i < N; ++i)
            process_order_[i] = static_cast<std::uint8_t>(i);
    }
    return true;
}

template <std::size_t N> void Voice<N>::Process() {
    const std::size_t frames = Audio::frames();

    // Gate and pitch are voice-level inputs. The pin system doesn't support
    // input-to-input wiring, so we push the values to children each buffer.
    Audio gate  = gate_in_->Read();
    Audio pitch = cv_pitch_in_->Read();

    for (std::size_t i = 0; i < N; ++i) {
        envelopes_[i]->gate_in_->Write(gate);
        envelopes_[i]->Process();
        operators_[i]->cv_pitch_in_->Write(pitch);
        operators_[i]->cv_level_in_->Write(envelopes_[i]->cv_out_->Read());
    }

    // Process operators in topological order, routing phase modulation from
    // each completed modulator to the operators it drives.
    std::array<Audio, N> op_out{};

    for (std::size_t oi = 0; oi < N; ++oi) {
        const std::size_t i = process_order_[oi];

        // Accumulate phase modulation from all modulators already processed.
        Audio phase_sum(ChannelLayout::kMono);
        fy_real *phase_data = phase_sum.array().data();
        std::fill(phase_data, phase_data + frames, fy_real{0});

        for (std::size_t j = 0; j < N; ++j) {
            if (j != i && algorithm_.Modulates(j, i) && !op_out[j].Empty()) {
                const fy_real *src = op_out[j].array().data();
                for (std::size_t f = 0; f < frames; ++f)
                    phase_data[f] += src[f];
            }
        }

        operators_[i]->cv_phase_in_->Write(phase_sum);
        operators_[i]->Process();
        op_out[i] = operators_[i]->audio_out_->Read();
    }

    // Sum carrier outputs into the voice output.
    Audio out(ChannelLayout::kMono);
    fy_real *out_data = out.array().data();
    std::fill(out_data, out_data + frames, fy_real{0});

    for (std::size_t i = 0; i < N; ++i) {
        if (!algorithm_.IsCarrier(i) || op_out[i].Empty())
            continue;
        const fy_real *src = op_out[i].array().data();
        for (std::size_t f = 0; f < frames; ++f)
            out_data[f] += src[f];
    }

    audio_out_->Write(out);
}

template <std::size_t N> void Voice<N>::RebuildProcessOrder() {
    if (!TopoSort(algorithm_, process_order_)) {
        for (std::size_t i = 0; i < N; ++i)
            process_order_[i] = static_cast<std::uint8_t>(i);
    }
}

// Explicit instantiations for the sizes you're likely to use.
template class Voice<2>;
template class Voice<4>;
template class Voice<6>;

} // namespace augr::fm
