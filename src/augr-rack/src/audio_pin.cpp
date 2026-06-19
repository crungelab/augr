#include <augr/rack/audio_pin.h>

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