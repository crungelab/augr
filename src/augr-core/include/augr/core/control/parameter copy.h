#pragma once

// parameter.h
//
// VST3-inspired Parameter architecture for augr-core.
//
// Design principles:
//   - Parameter is the single source of truth for a control value
//   - It owns a BindingT<fy_real> rather than a raw float* — any binding
//     type works: ZoneBinding (Faust), ValueBinding, CallbackBinding, etc.
//   - GetNormalized/SetNormalized bridge to ImGui's [0..1] world
//   - GetValue/SetValue operate in internal space (dB, Hz, etc.)
//   - Controls are pure views — they hold a Parameter* and nothing else
//   - The Rack/graph layer can enumerate and drive Parameters directly,
//     independently of any UI
//
// Hierarchy:
//   Parameter            (abstract base)
//   ├── LinearParameter
//   ├── DecibelParameter
//   └── FrequencyParameter
//
// Factory:
//   Parameter::Make(label, meta, binding, init, min, max, step)
//   → picks the right subclass from ControlMeta::Unit()

#include <algorithm>
#include <cassert>
#include <cmath>
#include <format>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include <augr/core/binding.h>
#include <augr/core/control/control_meta.h>

namespace augr {

// ---------------------------------------------------------------------------
// Parameter  (abstract base)
// ---------------------------------------------------------------------------

class Parameter {
public:
    using BindingPtr = std::unique_ptr<BindingT<fy_real>>;

    Parameter(std::string label, ControlMeta meta, BindingPtr binding,
              fy_real init, fy_real min, fy_real max, fy_real step)
        : label_(std::move(label)), meta_(std::move(meta)),
          binding_(std::move(binding)), init_(init), min_(min), max_(max),
          step_(step), unit_(meta_.Unit()) {}

    virtual ~Parameter() = default;

    // Non-copyable, non-movable — held by pointer throughout lifetime
    Parameter(const Parameter &) = delete;
    Parameter &operator=(const Parameter &) = delete;

    // -- Core interface (subclasses implement these) --------------------------

    // Current value → normalized [0..1] for ImGui
    virtual fy_real GetNormalized() const = 0;

    // Normalized [0..1] from ImGui → snap/clamp → write to binding
    virtual void SetNormalized(fy_real pos) = 0;

    // Human-readable string with units, e.g. "-6.0 dB", "440.0 Hz"
    virtual std::string Format() const = 0;

    // -- Value access in internal space ---------------------------------------

    fy_real GetValue() const { return binding_->get(); }

    void SetValue(fy_real internal) {
        binding_->set(SnapAndClamp(internal));
        NotifyObservers();
    }

    void ResetToInit() {
        binding_->set(init_);
        NotifyObservers();
    }

    // -- Observers ------------------------------------------------------------
    // Lightweight callback list — lets controls/displays react to
    // programmatic changes (automation, OSC, preset recall, etc.)

    using Observer = std::function<void(fy_real value)>;

    void AddObserver(Observer cb) { observers_.push_back(std::move(cb)); }

    // -- Accessors ------------------------------------------------------------

    const std::string &label() const { return label_; }
    const ControlMeta &meta() const { return meta_; }
    ControlUnit unit() const { return unit_; }
    fy_real init() const { return init_; }
    fy_real min() const { return min_; }
    fy_real max() const { return max_; }
    fy_real step() const { return step_; }
    bool IsKnob() const { return meta_.IsKnob(); }

    // Normalized positions of init/min/max — useful for tick marks
    fy_real NormalizedInit() const { return GetNormalizedFor(init_); }
    fy_real NormalizedMin() const { return GetNormalizedFor(min_); }
    fy_real NormalizedMax() const { return GetNormalizedFor(max_); }

    // -- Factory --------------------------------------------------------------

    static std::unique_ptr<Parameter>
    Make(std::string label, ControlMeta meta, BindingPtr binding,
         fy_real init, fy_real min, fy_real max, fy_real step);

protected:
    // Snap to step grid then clamp — always operates in internal space
    fy_real SnapAndClamp(fy_real value) const {
        if (step_ > fy_real{0})
            value = min_ + std::round((value - min_) / step_) * step_;
        return std::clamp(value, min_, max_);
    }

    void NotifyObservers() {
        fy_real v = binding_->get();
        for (auto &cb : observers_)
            cb(v);
    }

    // Used by NormalizedInit/Min/Max without triggering SetNormalized
    // side effects
    virtual fy_real GetNormalizedFor(fy_real value) const = 0;

    std::string label_;
    ControlMeta meta_;
    BindingPtr binding_;
    fy_real init_;
    fy_real min_;
    fy_real max_;
    fy_real step_;
    ControlUnit unit_;
    std::vector<Observer> observers_;
};

// ---------------------------------------------------------------------------
// LinearParameter
// ---------------------------------------------------------------------------

class LinearParameter : public Parameter {
public:
    LinearParameter(std::string label, ControlMeta meta, BindingPtr binding,
                    fy_real init, fy_real min, fy_real max, fy_real step,
                    const char *suffix = "")
        : Parameter(std::move(label), std::move(meta), std::move(binding), init,
                    min, max, step),
          suffix_(suffix) {}

    fy_real GetNormalized() const override {
        return GetNormalizedFor(binding_->get());
    }

    void SetNormalized(fy_real pos) override {
        binding_->set(SnapAndClamp(min_ + pos * (max_ - min_)));
        NotifyObservers();
    }

    std::string Format() const override {
        int decimals = (step_ < fy_real{0.1}) ? 2
                       : (step_ < fy_real{1}) ? 1
                                              : 0;
        return std::format("{:.{}f}{}", binding_->get(), decimals, suffix_);
    }

protected:
    fy_real GetNormalizedFor(fy_real value) const override {
        if (max_ == min_)
            return fy_real{0};
        return (value - min_) / (max_ - min_);
    }

private:
    const char *suffix_; // e.g. " ms", "%", "" — set by Make()
};

// ---------------------------------------------------------------------------
// DecibelParameter
// ---------------------------------------------------------------------------
//
// dB is already a log scale so the slider moves linearly across the dB range.
// Differences from LinearParameter:
//   - Display format: +/- sign, "dB" suffix, "-inf dB" at the floor
//   - Convention: min_ is the "silence" floor

class DecibelParameter : public Parameter {
public:
    using Parameter::Parameter;

    fy_real GetNormalized() const override {
        return GetNormalizedFor(binding_->get());
    }

    void SetNormalized(fy_real pos) override {
        binding_->set(SnapAndClamp(min_ + pos * (max_ - min_)));
        NotifyObservers();
    }

    std::string Format() const override {
        fy_real v = binding_->get();
        if (v <= min_)
            return "-inf dB";
        return std::format("{:+.1f} dB", v);
    }

protected:
    fy_real GetNormalizedFor(fy_real value) const override {
        if (max_ == min_)
            return fy_real{0};
        return (value - min_) / (max_ - min_);
    }
};

// ---------------------------------------------------------------------------
// FrequencyParameter
// ---------------------------------------------------------------------------
//
// Hz perception is logarithmic (pitch), so we use a true log mapping.
// One octave (e.g. 100→200 Hz) occupies the same slider distance as
// any other octave (e.g. 1000→2000 Hz).

class FrequencyParameter : public Parameter {
public:
    FrequencyParameter(std::string label, ControlMeta meta,
                       BindingPtr binding, fy_real init, fy_real min,
                       fy_real max, fy_real step)
        : Parameter(std::move(label), std::move(meta), std::move(binding), init,
                    min, max, step) {
        assert(min > fy_real{0} && max > fy_real{0});
        log_ratio_ = std::log(max / min);
    }

    fy_real GetNormalized() const override {
        return GetNormalizedFor(binding_->get());
    }

    void SetNormalized(fy_real pos) override {
        binding_->set(SnapAndClamp(min_ * std::exp(pos * log_ratio_)));
        NotifyObservers();
    }

    std::string Format() const override {
        fy_real v = binding_->get();
        if (v >= fy_real{1000})
            return std::format("{:.2f} kHz", v / fy_real{1000});
        return std::format("{:.1f} Hz", v);
    }

protected:
    fy_real GetNormalizedFor(fy_real value) const override {
        if (log_ratio_ == fy_real{0})
            return fy_real{0};
        return std::log(value / min_) / log_ratio_;
    }

private:
    fy_real log_ratio_; // cached log(max/min)
};

// ---------------------------------------------------------------------------
// Parameter::Make  (factory — defined here after all subclasses)
// ---------------------------------------------------------------------------

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
        // min == 0: fall through to linear (guard against log(0))
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
