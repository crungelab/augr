#include <augr/rack/pin/mixing_audio_input.h>

namespace augr {

/*
Audio MixingAudioInput::Reduce() const {
    Audio mixed = slots_[0]->Read();
    for (size_t i = 1; i < slots_.size(); ++i)
        mixed.array() += slots_[i]->Read().array();

    // Headroom scales with the number of connected carriers so a single
    // carrier isn't needlessly quietened while three or four carriers
    // summing don't clip. sqrt(N) rather than 1/N: carriers are rarely
    // fully phase-correlated, so a straight 1/N divide is overly
    // conservative and makes multi-carrier patches sound weak relative
    // to single-carrier ones.
    const float n = static_cast<float>(slots_.size());
    const float headroom = 1.0f / std::sqrt(n);
    mixed.array() *= headroom;

    // Hard safety clamp — never let true digital wraparound distortion
    // through, even if a patch (e.g. extreme feedback) still exceeds the
    // headroom estimate.
    mixed.array() = xt::clip(mixed.array(), fy_real(-1), fy_real(1));

    return mixed;
}
*/

Audio MixingAudioInput::Reduce() const {
    Audio mixed = slots_[0]->Read();
    for (size_t i = 1; i < slots_.size(); ++i)
        mixed.array() += slots_[i]->Read().array();

    // Headroom scales with the number of connected carriers so a single
    // carrier isn't needlessly quietened while three or four carriers
    // summing don't clip. sqrt(N) rather than 1/N: carriers are rarely
    // fully phase-correlated, so a straight 1/N divide is overly
    // conservative and makes multi-carrier patches sound weak relative
    // to single-carrier ones.
    const float n = static_cast<float>(slots_.size());
    const float headroom = 1.0f / std::sqrt(n);
    mixed.array() *= headroom;

    // Hard safety clamp — never let true digital wraparound distortion
    // through, even if a patch (e.g. extreme feedback) still exceeds the
    // headroom estimate.
    mixed.array() = xt::clip(mixed.array(), fy_real(-1), fy_real(1));

    return mixed;
}

/*
Audio MixingAudioInput::Reduce() const {
    if (slots_.empty()) {
        return Audio();
    }

    Audio mixed = Audio(layout_);
    mixed.array().fill(0);

    for (auto *slot : slots_) {
        auto slot_data = slot->Read();
        if (slot_data.Empty())
            continue;
        mixed.array() += slot_data.array();
    }

    fy_real *d = mixed.array().data();
    const size_t n = mixed.array().size();

    for (size_t k = 0; k < n; ++k)
        d[k] = std::tanh(d[k]);

    return mixed;
}
*/

/*
Audio MixingAudioInput::Reduce() const {
    if (slots_.empty()) {
        return Audio();
    }

    Audio mixed = slots_[0]->Read();

    if (mixed.Empty()) {
        return Audio();
    }

    fy_real *d = mixed.array().data();
    const size_t n = mixed.array().size();

    for (size_t i = 1; i < slots_.size(); ++i) {
        const Audio voice = slots_[i]->Read();
        const fy_real *s = voice.array().data();
        for (size_t k = 0; k < n; ++k)
            d[k] += s[k];
    }

    for (size_t k = 0; k < n; ++k)
        d[k] = std::tanh(d[k]);

    return mixed;
}
*/

/*
Audio MixingAudioInput::Reduce() const {
    Audio mixed = Audio(layout_);
    mixed.array().fill(0);
    for (auto *slot : slots_) {
        auto slot_data = slot->Read();
        if (slot_data.Empty())
            continue;
        mixed.array() += slot_data.array();
    }

    const float n = static_cast<float>(slots_.size());
    const float headroom =
        1.0f / n; // was 1/sqrt(n) — too generous for coherent carriers
    mixed.array() *= headroom;
    return mixed;
}
*/

/*
Audio MixingAudioInput::Reduce() const {
    Audio mixed = Audio(layout_);
    mixed.array().fill(0);
    for (auto *slot : slots_) {
        auto slot_data = slot->Read();
        if (slot_data.Empty())
            continue;
        mixed.array() += slot_data.array();
    }

    // Fixed headroom rather than 1/N — avoids quietening
    // single-carrier algorithms while preventing multi-carrier clipping.
    constexpr float kHeadroom = 0.25f;
    mixed.array() *= kHeadroom;
    return mixed;
}
*/

/*
Audio MixingAudioInput::Reduce() const {
    Audio mixed = slots_[0]->Read();
    for (size_t i = 1; i < slots_.size(); ++i)
        mixed.array() += slots_[i]->Read().array();

    // Fixed headroom rather than 1/N — avoids quietening
    // single-carrier algorithms while preventing multi-carrier clipping.
    constexpr float kHeadroom = 0.25f;
    mixed.array() *= kHeadroom;
    return mixed;
}
*/

} // namespace augr