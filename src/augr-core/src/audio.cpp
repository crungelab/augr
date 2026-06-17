#include <cstring>
#include <memory>

#include <augr/audio.h>

unsigned int Audio::frames_ = 512;
unsigned int Audio::sample_rate_ = 44100;

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

Audio::Audio(ChannelLayout layout) : layout_(layout) {
    switch (layout) {
    case ChannelLayout::kMono:
        impl_ = std::make_unique<AudioImplT<1>>();
        break;
    case ChannelLayout::kStereo:
        impl_ = std::make_unique<AudioImplT<2>>();
        break;
    case ChannelLayout::kQuad:
        impl_ = std::make_unique<AudioImplT<4>>();
        break;
    case ChannelLayout::k5_1:
        impl_ = std::make_unique<AudioImplT<6>>();
        break;
    case ChannelLayout::k7_1:
        impl_ = std::make_unique<AudioImplT<8>>();
        break;
    case ChannelLayout::kNull:
    default:
        impl_ = nullptr;
        break;
    }
}

Audio::Audio(fy_real *raw, size_t nChannels) {
    layout_ = ChannelCountToLayout(static_cast<int>(nChannels));
    switch (nChannels) {
    case 1:
        impl_ = std::make_unique<AudioImplAdapted<1>>(raw);
        break;
    case 2:
        impl_ = std::make_unique<AudioImplAdapted<2>>(raw);
        break;
    case 4:
        impl_ = std::make_unique<AudioImplAdapted<4>>(raw);
        break;
    case 6:
        impl_ = std::make_unique<AudioImplAdapted<6>>(raw);
        break;
    case 8:
        impl_ = std::make_unique<AudioImplAdapted<8>>(raw);
        break;
    default:
        impl_ = nullptr;
        break;
    }
}

Audio::Audio(const Audio &other) : layout_(other.layout_) {
    if (other.impl_) {
        impl_ = other.impl_->Clone();
    }
}

Audio &Audio::operator=(const Audio &other) {
    if (this != &other) {
        layout_ = other.layout_;
        impl_ = other.impl_ ? other.impl_->Clone() : nullptr;
    }
    return *this;
}

// ---------------------------------------------------------------------------
// I/O
// ---------------------------------------------------------------------------

void Audio::WritePlanar(fy_buffer_t out, fy_real scale) {
    if (!impl_)
        return;

    auto &arr = array();
    const size_t C = nChannels();
    fy_real *base = arr.data(); // [C, frames_], row-major
    const size_t F = frames_;

    for (size_t ch = 0; ch < C; ++ch) {
        const fy_real *src = base + ch * F;
        if (scale == fy_real(1)) {
            std::memcpy(out, src, F * sizeof(fy_real));
        } else {
            for (size_t i = 0; i < F; ++i)
                out[i] = src[i] * scale;
        }
        out += F;
    }
}

// ---------------------------------------------------------------------------
// Layout utilities
// ---------------------------------------------------------------------------

ChannelLayout Audio::ChannelCountToLayout(int count) {
    switch (count) {
    case 1:
        return ChannelLayout::kMono;
    case 2:
        return ChannelLayout::kStereo;
    case 4:
        return ChannelLayout::kQuad;
    case 6:
        return ChannelLayout::k5_1;
    case 8:
        return ChannelLayout::k7_1;
    default:
        return ChannelLayout::kNull;
    }
}

int Audio::LayoutToChannelCount(ChannelLayout layout) {
    switch (layout) {
    case ChannelLayout::kMono:
        return 1;
    case ChannelLayout::kStereo:
        return 2;
    case ChannelLayout::kQuad:
        return 4;
    case ChannelLayout::k5_1:
        return 6;
    case ChannelLayout::k7_1:
        return 8;
    case ChannelLayout::kNull:
    default:
        return 0;
    }
}

// ---------------------------------------------------------------------------
// Layout conversion
// ---------------------------------------------------------------------------

Audio Audio::Convert(ChannelLayout toLayout) const {
    switch (layout_) {
    case ChannelLayout::kMono:
        if (toLayout == ChannelLayout::kStereo)
            return ConvertMonoToStereo();
        return Audio();
    case ChannelLayout::kStereo:
        if (toLayout == ChannelLayout::kMono)
            return ConvertStereoToMono();
        return Audio();
    default:
        return Audio();
    }
}

Audio Audio::ConvertMonoToStereo() const {
    Audio out(ChannelLayout::kStereo);
    const fy_real *src = array().data(); // [1, F]
    const size_t F = frames();

    auto dst = out.buffers();
    // -3 dB pan-law to maintain perceived loudness
    constexpr fy_real kPanScale = fy_real(0.70710678);
    for (size_t i = 0; i < F; ++i) {
        fy_real s = src[i] * kPanScale;
        dst[0][i] = s;
        dst[1][i] = s;
    }
    return out;
}

Audio Audio::ConvertStereoToMono() const {
    Audio out(ChannelLayout::kMono);
    const size_t F = frames();
    const fy_real *L = array().data() + 0 * F;
    const fy_real *R = array().data() + 1 * F;

    fy_real *M = out.buffers()[0];
    for (size_t i = 0; i < F; ++i)
        M[i] = (L[i] + R[i]) * fy_real(0.5);
    return out;
}