#pragma once

#include "config.h"

#include <array>
#include <cstdlib>
#include <memory>
#include <vector>

#include <xtensor/containers/xadapt.hpp>
#include <xtensor/containers/xarray.hpp>

// ---------------------------------------------------------------------------
// Channel layout
// ---------------------------------------------------------------------------

enum class ChannelLayout { kNull, kMono, kStereo, kQuad, k5_1, k7_1 };

// ---------------------------------------------------------------------------
// AudioImpl: type-erased base for channel-count-specialized storage
// ---------------------------------------------------------------------------

class AudioImpl {
public:
    virtual ~AudioImpl() = default;

    virtual size_t nChannels() const = 0;
    virtual fy_buffers_t buffers() = 0;

    // Deep-copy clone — required for Audio's value semantics.
    virtual std::unique_ptr<AudioImpl> Clone() const = 0;

    xt::xarray<fy_real> &array() { return array_; }
    const xt::xarray<fy_real> &array() const { return array_; }

protected:
    xt::xarray<fy_real> array_;
};

using AudioImplPtr = std::unique_ptr<AudioImpl>;

// ---------------------------------------------------------------------------
// Audio: value-typed handle to a channel-specialized impl
// ---------------------------------------------------------------------------

class Audio {
public:
    explicit Audio(ChannelLayout layout = ChannelLayout::kNull);
    Audio(fy_real *raw, size_t channels);

    // Value semantics: deep copy.
    Audio(const Audio &other);
    Audio &operator=(const Audio &other);
    Audio(Audio &&) noexcept = default;
    Audio &operator=(Audio &&) noexcept = default;
    ~Audio() = default;

    void WritePlanar(fy_buffer_t out, fy_real scale = 1.0);

    // Layout conversion
    static ChannelLayout ChannelCountToLayout(int count);
    static int LayoutToChannelCount(ChannelLayout layout);
    Audio Convert(ChannelLayout toLayout) const;
    Audio ConvertMonoToStereo() const;
    Audio ConvertStereoToMono() const;

    // Arithmetic
    Audio operator+(const Audio &other) const {
        if (Empty())
            return other; // returns a deep copy via copy ctor
        if (other.Empty())
            return *this;    // ditto
        Audio result(*this); // deep copy
        result.array() += other.array();
        return result;
    }

    Audio &operator+=(const Audio &other) {
        if (other.Empty())
            return *this;
        if (Empty()) {
            *this = other; // deep copy via copy assignment
            return *this;
        }
        array() += other.array();
        return *this;
    }

    bool Empty() const { return !impl_; }

    // Audio-format globals — set once at engine init.
    static unsigned int sample_rate() { return sample_rate_; }
    static unsigned int frames() { return frames_; }
    static void set_sample_rate(unsigned int rate) { sample_rate_ = rate; }
    static void set_frames(unsigned int n) { frames_ = n; }

    // Array / buffer access
    xt::xarray<fy_real> &array() { return impl_->array(); }
    const xt::xarray<fy_real> &array() const { return impl_->array(); }
    fy_buffers_t buffers() { return impl_->buffers(); }

    size_t nChannels() const { return impl_ ? impl_->nChannels() : 0; }
    ChannelLayout layout() const { return layout_; }

    AudioImpl &impl() { return *impl_; }
    const AudioImpl &impl() const { return *impl_; }

private:
    static unsigned int frames_;
    static unsigned int sample_rate_;

    ChannelLayout layout_;
    AudioImplPtr impl_;
};

// ---------------------------------------------------------------------------
// AudioImplT<N>: owning storage, allocates its own buffer
// ---------------------------------------------------------------------------

template <size_t N> class AudioImplT : public AudioImpl {
public:
    AudioImplT() {
        std::vector<size_t> shape = {N, Audio::frames()};
        array_ = xt::xarray<fy_real>::from_shape(shape);
        RebindBuffers();
    }

    size_t nChannels() const override { return N; }
    fy_buffers_t buffers() override { return buffers_.data(); }

    std::unique_ptr<AudioImpl> Clone() const override {
        auto copy = std::make_unique<AudioImplT<N>>();
        copy->array_ = array_; // xtensor deep copy
        copy->RebindBuffers();
        return copy;
    }

private:
    void RebindBuffers() {
        fy_real *base = array_.data();
        for (size_t i = 0; i < N; ++i) {
            buffers_[i] = base + i * Audio::frames();
        }
    }

    std::array<fy_buffer_t, N> buffers_{};
};

// ---------------------------------------------------------------------------
// AudioImplAdapted<N>: non-owning view over an externally-owned buffer.
//
// Stores the raw pointer and per-channel buffer pointers directly.
// Does NOT populate the base array_ — callers must access data through
// buffers() rather than array() on adapted impls.
// ---------------------------------------------------------------------------

template <size_t N>
class AudioImplAdapted : public AudioImpl {
public:
    explicit AudioImplAdapted(fy_real *raw) : raw_(raw) {
        const size_t F = Audio::frames();
        for (size_t i = 0; i < N; ++i) {
            buffers_[i] = raw + i * F;
        }
    }

    size_t nChannels() const override { return N; }
    fy_buffers_t buffers() override { return buffers_.data(); }

    std::unique_ptr<AudioImpl> Clone() const override {
        // Cloning a view materializes an owning copy.
        const size_t F = Audio::frames();
        auto copy = std::make_unique<AudioImplT<N>>();
        std::memcpy(copy->array().data(), raw_, N * F * sizeof(fy_real));
        return copy;
    }

private:
    fy_real *raw_;
    std::array<fy_buffer_t, N> buffers_{};
};