#pragma once

#include <cstdlib>
#undef min
#undef max

// #include <xtensor/xarray.hpp>
#include "config.h"
#include <xtensor/containers/xadapt.hpp>

class AudioImpl {
public:
    virtual size_t nChannels() = 0;
    virtual fy_buffers_t buffers() = 0;

    // Accessors
    xt::xarray<fy_real> &array() { return array_; }
    const xt::xarray<fy_real> &array() const { return array_; }

    // Data members
    std::vector<size_t> shape_;
    xt::xarray<fy_real> array_;
};

typedef std::shared_ptr<AudioImpl> AudioImplPtr;

enum class ChannelLayout { kNull, kMono, kStereo, kQuad, k5_1 };

class Audio {
public:
    Audio(const Audio &other);
    Audio(ChannelLayout layout = ChannelLayout::kNull);
    Audio(fy_real *raw, size_t channels);
    //
    void WritePlanar(fy_buffer_t out, fy_real scale = 1.0);
    // Utilities
    static ChannelLayout ChannelCountToLayout(int count);
    static int LayoutToChannelCount(ChannelLayout layout);
    Audio Convert(ChannelLayout toLayout);
    Audio ConvertMonoToStereo();
    Audio ConvertStereoToMono();
    // Accessors
    static unsigned int sampleRate() { return sampleRate_; }
    static unsigned int frames() { return frames_; }

    xt::xarray<fy_real> &array() { return impl_->array(); }
    const xt::xarray<fy_real> &array() const { return impl_->array(); }

    fy_buffers_t buffers() { return impl_->buffers(); }

    size_t nChannels() { return impl_->nChannels(); }
    AudioImpl &impl() { return *impl_.get(); }
    // Data members
    static unsigned int frames_;
    static unsigned int sampleRate_;
    ChannelLayout layout_;
    AudioImplPtr impl_;
};

template <size_t N> class AudioImplT : public AudioImpl {
public:
    AudioImplT() {
        shape_ = {N, Audio::frames()};
        array_ = xt::xarray<fy_real>(shape_);

        fy_real *base = array_.data(); // contiguous
        for (size_t i = 0; i < N; ++i) {
            buffers_[i] = base + i * Audio::frames(); // start of channel i
        }
    }

    AudioImplT(fy_real *raw) {
        size_t size = Audio::frames() * N;
        shape_ = {N, Audio::frames()};

        array_ = xt::adapt(raw, size, xt::no_ownership(), shape_);

        fy_real *base = array_.data(); // points into raw
        for (size_t i = 0; i < N; ++i) {
            buffers_[i] = base + i * Audio::frames();
        }
    }

    size_t nChannels_ = N;
    fy_buffer_t buffers_[N];

    // Accessors
    size_t nChannels() override { return nChannels_; }
    fy_buffers_t buffers() override { return buffers_; }
};
