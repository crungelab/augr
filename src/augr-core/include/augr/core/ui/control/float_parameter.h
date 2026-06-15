#ifndef AUGR_CORE_PARAMETER_H_
#define AUGR_CORE_PARAMETER_H_

#include <algorithm>
#include <cassert>
#include <cmath>
#include <format>
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <augr/core/ui/control/parameter.h>

namespace augr {

// ---------------------------------------------------------------------------
// FloatParameter: numeric-range scalar parameter. Abstract -- concrete
// subclasses (Linear/Decibel/Frequency) pick the mapping.
// ---------------------------------------------------------------------------
class FloatParameter : public ParameterT<fy_real> {
public:
    using BindingPtr = ParameterT<fy_real>::BindingPtr;

    FloatParameter(std::string label, ControlMeta meta, BindingPtr binding,
                   fy_real init, fy_real min, fy_real max, fy_real step)
        : ParameterT<fy_real>(std::move(label), std::move(meta),
                              std::move(binding)),
          init_(init), min_(min), max_(max), step_(step) {}

    // Writes through snap/clamp, then notifies. Overrides BoundControl.
    void set_value(const fy_real &value) override {
        if (this->binding_) {
            auto snapped = SnapAndClamp(value);
            this->binding_->set(snapped);
            this->on_change(snapped);
        }
    }

    void ResetToInit() override { set_value(init_); }

    fy_real init() const { return init_; }
    fy_real min() const { return min_; }
    fy_real max() const { return max_; }
    fy_real step() const { return step_; }

    fy_real NormalizedInit() const { return GetNormalizedFor(init_); }
    fy_real NormalizedMin() const { return GetNormalizedFor(min_); }
    fy_real NormalizedMax() const { return GetNormalizedFor(max_); }

    // Factory: picks the concrete subclass based on meta.Unit().
    static std::unique_ptr<FloatParameter>
    Make(std::string label, ControlMeta meta, BindingPtr binding, fy_real init,
         fy_real min, fy_real max, fy_real step);

protected:
    fy_real SnapAndClamp(fy_real v) const {
        if (step_ > fy_real{0}) {
            v = min_ + std::round((v - min_) / step_) * step_;
        }
        return std::clamp(v, min_, max_);
    }

    virtual fy_real GetNormalizedFor(fy_real v) const = 0;

private:
    fy_real init_, min_, max_, step_;
};

// ---------------------------------------------------------------------------
// Concrete float parameters. Bodies unchanged from the original -- only the
// base class name (Parameter -> FloatParameter) differs.
// ---------------------------------------------------------------------------
class LinearParameter : public FloatParameter {
public:
    LinearParameter(std::string label, ControlMeta meta, BindingPtr binding,
                    fy_real init, fy_real min, fy_real max, fy_real step,
                    std::string suffix = "")
        : FloatParameter(std::move(label), std::move(meta), std::move(binding),
                         init, min, max, step),
          suffix_(std::move(suffix)) {}

    fy_real GetNormalized() const override { return GetNormalizedFor(value()); }

    void SetNormalized(fy_real pos) override {
        set_value(min() + pos * (max() - min()));
    }

    std::string Format() const override {
        const int decimals = (step() < fy_real{0.1}) ? 2
                             : (step() < fy_real{1}) ? 1
                                                     : 0;
        return std::format("{:.{}f}{}", value(), decimals, suffix_);
    }

protected:
    fy_real GetNormalizedFor(fy_real v) const override {
        if (max() == min())
            return fy_real{0};
        return (v - min()) / (max() - min());
    }

private:
    std::string suffix_;
};

class DecibelParameter : public FloatParameter {
public:
    using FloatParameter::FloatParameter;

    fy_real GetNormalized() const override { return GetNormalizedFor(value()); }

    void SetNormalized(fy_real pos) override {
        set_value(min() + pos * (max() - min()));
    }

    std::string Format() const override {
        const fy_real v = value();
        if (v <= min())
            return "-inf dB";
        return std::format("{:+.1f} dB", v);
    }

protected:
    fy_real GetNormalizedFor(fy_real v) const override {
        if (max() == min())
            return fy_real{0};
        return (v - min()) / (max() - min());
    }
};

class FrequencyParameter : public FloatParameter {
public:
    FrequencyParameter(std::string label, ControlMeta meta, BindingPtr binding,
                       fy_real init, fy_real min, fy_real max, fy_real step)
        : FloatParameter(std::move(label), std::move(meta), std::move(binding),
                         init, min, max, step) {
        assert(min > fy_real{0} && max > fy_real{0});
        log_ratio_ = std::log(max / min);
    }

    fy_real GetNormalized() const override { return GetNormalizedFor(value()); }

    void SetNormalized(fy_real pos) override {
        set_value(min() * std::exp(pos * log_ratio_));
    }

    std::string Format() const override {
        const fy_real v = value();
        if (v >= fy_real{1000}) {
            return std::format("{:.2f} kHz", v / fy_real{1000});
        }
        return std::format("{:.1f} Hz", v);
    }

protected:
    fy_real GetNormalizedFor(fy_real v) const override {
        if (log_ratio_ == fy_real{0})
            return fy_real{0};
        return std::log(v / min()) / log_ratio_;
    }

private:
    fy_real log_ratio_;
};

// Factory body -- unchanged logic, just returns FloatParameter.
inline std::unique_ptr<FloatParameter>
FloatParameter::Make(std::string label, ControlMeta meta, BindingPtr binding,
                     fy_real init, fy_real min, fy_real max, fy_real step) {
    switch (meta.Unit()) {
    case ControlUnit::kDecibel:
        return std::make_unique<DecibelParameter>(
            std::move(label), std::move(meta), std::move(binding), init, min,
            max, step);

    case ControlUnit::kHertz:
        if (min > fy_real{0}) {
            return std::make_unique<FrequencyParameter>(
                std::move(label), std::move(meta), std::move(binding), init,
                min, max, step);
        }
        [[fallthrough]];

    case ControlUnit::kMilliseconds:
        return std::make_unique<LinearParameter>(
            std::move(label), std::move(meta), std::move(binding), init, min,
            max, step, " ms");

    case ControlUnit::kSeconds:
        return std::make_unique<LinearParameter>(
            std::move(label), std::move(meta), std::move(binding), init, min,
            max, step, " s");

    case ControlUnit::kPercent:
        return std::make_unique<LinearParameter>(
            std::move(label), std::move(meta), std::move(binding), init, min,
            max, step, "%");

    case ControlUnit::kSemitones:
        return std::make_unique<LinearParameter>(
            std::move(label), std::move(meta), std::move(binding), init, min,
            max, step, " st");

    case ControlUnit::kBpm:
        return std::make_unique<LinearParameter>(
            std::move(label), std::move(meta), std::move(binding), init, min,
            max, step, " bpm");

    default:
        return std::make_unique<LinearParameter>(
            std::move(label), std::move(meta), std::move(binding), init, min,
            max, step);
    }
}

} // namespace augr

#endif // AUGR_CORE_PARAMETER_H_