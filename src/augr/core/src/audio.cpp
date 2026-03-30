#include <cstring>

#include <augr/core/audio.h>

unsigned int Audio::frames_ = 512;
unsigned int Audio::sampleRate_ = 44100;

Audio::Audio(const Audio &other) {
    layout_ = other.layout_;
    frames_ = other.frames_;
    impl_ = other.impl_;
}
Audio::Audio(fy_real *raw, size_t nChannels) {
    layout_ = ChannelCountToLayout(nChannels);
    switch (nChannels) {
    case 1:
        // layout_ = ChannelLayout::kMono;
        impl_ = AudioImplPtr(new AudioImplT<1>(raw));
        break;
    case 2:
        // layout_ = ChannelLayout::kStereo;
        impl_ = AudioImplPtr(new AudioImplT<2>(raw));
        break;
    case 4:
        // layout_ = ChannelLayout::kQuad;
        impl_ = AudioImplPtr(new AudioImplT<4>(raw));
        break;
    case 6:
        // layout_ = ChannelLayout::k5_1;
        impl_ = AudioImplPtr(new AudioImplT<6>(raw));
        break;
    case 8:
        // layout_ = ChannelLayout::k7_1;
        impl_ = AudioImplPtr(new AudioImplT<8>(raw));
        break;
    default:
        // layout_ = ChannelLayout::kNull;
        impl_ = nullptr;
        break;
    }
}
Audio::Audio(ChannelLayout layout) : layout_(layout) {
    switch (layout) {
    case ChannelLayout::kMono:
        impl_ = AudioImplPtr(new AudioImplT<1>());
        break;
    case ChannelLayout::kStereo:
        impl_ = AudioImplPtr(new AudioImplT<2>());
        break;
    case ChannelLayout::kQuad:
        impl_ = AudioImplPtr(new AudioImplT<4>());
        break;
    case ChannelLayout::k5_1:
        impl_ = AudioImplPtr(new AudioImplT<6>());
        break;
    case ChannelLayout::k7_1:
        impl_ = AudioImplPtr(new AudioImplT<8>());
        break;
    default:
        impl_ = nullptr;
        break;
    }
}

void Audio::WritePlanar(fy_buffer_t out, fy_real scale) {
    auto &arr = array(); // reference, not copy
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
        out += F; // advance by F samples (not bytes)
    }
}

// Utilities
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
    default:
        return 0;
    }
}

Audio Audio::Convert(ChannelLayout toLayout) {
    switch (layout_) {
    case ChannelLayout::kMono: {
        switch (toLayout) {
        case ChannelLayout::kStereo:
            return ConvertMonoToStereo();

        default:
            return Audio();
        }
    }
    case ChannelLayout::kStereo: {
        switch (toLayout) {
        case ChannelLayout::kMono:
            return ConvertStereoToMono();

        default:
            return Audio();
        }
    }

    default:
        return Audio();
    }
}

Audio Audio::ConvertMonoToStereo() {
    Audio audio(ChannelLayout::kStereo);
    auto &srcArr = array(); // [1, F]
    const fy_real *src = srcArr.data();
    const size_t F = frames();

    auto dst = audio.buffers(); // two planar channel pointers
    // Optional -3 dB to maintain headroom
    for (size_t i = 0; i < F; ++i) {
        fy_real s = src[i] * fy_real(0.70710678);
        dst[0][i] = s;
        dst[1][i] = s;
    }
    return audio;
}

Audio Audio::ConvertStereoToMono() {
    Audio audio(ChannelLayout::kMono);
    auto &srcArr = array(); // [2, F]
    const size_t F = frames();
    const fy_real *L = srcArr.data() + 0 * F;
    const fy_real *R = srcArr.data() + 1 * F;

    fy_real *M = audio.buffers()[0];
    for (size_t i = 0; i < F; ++i)
        M[i] = (L[i] + R[i]) * fy_real(0.5);
    return audio;
}
