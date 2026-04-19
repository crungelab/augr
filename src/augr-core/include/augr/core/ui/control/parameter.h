#ifndef AUGR_CORE_PARAMETER_H_
#define AUGR_CORE_PARAMETER_H_

// VST3-inspired Parameter architecture for augr-core.
//
// Design principles:
//   - Parameter is the single source of truth for a control value.
//   - Parameter extends BoundControl<fy_real>, inheriting label/meta/binding
//     plumbing. It adds a normalized [0, 1] view for ImGui, snap/clamp on
//     write, range metadata (init/min/max/step), and observer notification.
//   - Any BindingT<fy_real> works: ZoneBinding (Faust), ValueBinding,
//     CallbackBinding, etc.
//   - GetNormalized() / SetNormalized() bridge to ImGui's [0, 1] world.
//   - value() / set_value() operate in internal space (dB, Hz, etc.).
//     set_value() is the overridden BoundControl hook; it snaps, clamps, and
//     notifies observers.
//   - Controls are pure views -- they hold a Parameter* and nothing else.
//   - The Rack/graph layer can enumerate and drive Parameters directly,
//     independently of any UI.
//
// Hierarchy:
//   BoundControl<fy_real>
//   +-- Parameter            (abstract)
//       |-- LinearParameter
//       |-- DecibelParameter
//       +-- FrequencyParameter
//
// Factory:
//   Parameter::Make(label, meta, binding, init, min, max, step)
//     picks the right subclass from ControlMeta::Unit().

#include <algorithm>
#include <cassert>
#include <cmath>
#include <format>
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <augr/core/binding.h>
#include <augr/core/ui/control/control.h>
#include <augr/core/ui/control/control_meta.h>

namespace augr {

// Abstract base class for all parameters.
class Parameter : public BoundControl<fy_real> {
public:
    using Observer = std::function<void(fy_real value)>;

    Parameter(std::string label, ControlMeta meta, BindingPtr binding,
              fy_real init, fy_real min, fy_real max, fy_real step)
        : BoundControl<fy_real>(std::move(label), std::move(meta),
                                std::move(binding)),
          init_(init), min_(min), max_(max), step_(step) {}

    ~Parameter() override = default;

    // Non-copyable, non-movable -- held by pointer throughout lifetime.
    Parameter(const Parameter &) = delete;
    Parameter &operator=(const Parameter &) = delete;
    Parameter(Parameter &&) = delete;
    Parameter &operator=(Parameter &&) = delete;

    // Core interface.
    //
    // Returns the current value mapped to [0, 1] for ImGui.
    virtual fy_real GetNormalized() const = 0;

    // Maps a normalized [0, 1] position from ImGui to internal space,
    // snaps/clamps, and writes it to the binding.
    virtual void SetNormalized(fy_real pos) = 0;

    // Returns a human-readable string with units, e.g. "-6.0 dB", "440.0 Hz".
    virtual std::string Format() const = 0;

    // Writes an internal-space value through snap/clamp and notifies observers.
    // Overrides BoundControl's trivial setter.
    void set_value(const fy_real &value) override {
        if (binding_) {
            binding_->set(SnapAndClamp(value));
            NotifyObservers();
        }
    }

    void ResetToInit() { set_value(init_); }

    // Registers a callback that fires on programmatic value changes
    // (automation, OSC, preset recall, etc.).
    void AddObserver(Observer cb) { observers_.push_back(std::move(cb)); }

    // Range accessors.
    fy_real init() const { return init_; }
    fy_real min() const { return min_; }
    fy_real max() const { return max_; }
    fy_real step() const { return step_; }
    bool IsKnob() const { return meta_.IsKnob(); }

    // Normalized positions of init/min/max. Useful for tick marks.
    fy_real NormalizedInit() const { return GetNormalizedFor(init_); }
    fy_real NormalizedMin() const { return GetNormalizedFor(min_); }
    fy_real NormalizedMax() const { return GetNormalizedFor(max_); }

    // Factory: picks the concrete subclass based on meta.Unit().
    static std::unique_ptr<Parameter> Make(std::string label, ControlMeta meta,
                                           BindingPtr binding, fy_real init,
                                           fy_real min, fy_real max,
                                           fy_real step);

protected:
    // Snaps to step grid then clamps. Always operates in internal space.
    fy_real SnapAndClamp(fy_real value) const {
        if (step_ > fy_real{0}) {
            value = min_ + std::round((value - min_) / step_) * step_;
        }
        return std::clamp(value, min_, max_);
    }

    void NotifyObservers() {
        const fy_real v = value();
        for (auto &cb : observers_) {
            cb(v);
        }
    }

    // Maps an internal-space value to [0, 1] without side effects. Used by
    // NormalizedInit()/Min()/Max() and by subclasses' GetNormalized().
    virtual fy_real GetNormalizedFor(fy_real value) const = 0;

private:
    fy_real init_;
    fy_real min_;
    fy_real max_;
    fy_real step_;
    std::vector<Observer> observers_;
};

// Linear-scale parameter. The slider moves linearly between min and max.
class LinearParameter : public Parameter {
public:
    LinearParameter(std::string label, ControlMeta meta, BindingPtr binding,
                    fy_real init, fy_real min, fy_real max, fy_real step,
                    std::string suffix = "")
        : Parameter(std::move(label), std::move(meta), std::move(binding), init,
                    min, max, step),
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
        if (max() == min()) {
            return fy_real{0};
        }
        return (v - min()) / (max() - min());
    }

private:
    std::string suffix_; // e.g. " ms", "%", "". Set by Make().
};

// Decibel-scale parameter.
//
// dB is already a log scale so the slider moves linearly across the dB range.
// Differences from LinearParameter:
//   - Display format: +/- sign, "dB" suffix, "-inf dB" at the floor.
//   - Convention: min is the "silence" floor.
class DecibelParameter : public Parameter {
public:
    using Parameter::Parameter;

    fy_real GetNormalized() const override { return GetNormalizedFor(value()); }

    void SetNormalized(fy_real pos) override {
        set_value(min() + pos * (max() - min()));
    }

    std::string Format() const override {
        const fy_real v = value();
        if (v <= min()) {
            return "-inf dB";
        }
        return std::format("{:+.1f} dB", v);
    }

protected:
    fy_real GetNormalizedFor(fy_real v) const override {
        if (max() == min()) {
            return fy_real{0};
        }
        return (v - min()) / (max() - min());
    }
};

// Frequency-scale parameter.
//
// Hz perception is logarithmic (pitch), so a true log mapping is used.
// One octave (e.g. 100 -> 200 Hz) occupies the same slider distance as any
// other octave (e.g. 1000 -> 2000 Hz).
class FrequencyParameter : public Parameter {
public:
    FrequencyParameter(std::string label, ControlMeta meta, BindingPtr binding,
                       fy_real init, fy_real min, fy_real max, fy_real step)
        : Parameter(std::move(label), std::move(meta), std::move(binding), init,
                    min, max, step) {
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
        if (log_ratio_ == fy_real{0}) {
            return fy_real{0};
        }
        return std::log(v / min()) / log_ratio_;
    }

private:
    fy_real log_ratio_; // Cached log(max/min).
};

// Factory definition. Placed after all subclasses so they are complete types.
inline std::unique_ptr<Parameter>
Parameter::Make(std::string label, ControlMeta meta, BindingPtr binding,
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
        // min == 0: fall through to linear (guard against log(0)).
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