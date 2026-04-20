#pragma once

// VST3-inspired Parameter architecture for augr-core.
//
// Hierarchy:
//   Control
//   └── Parameter                         (abstract; normalized I/O, format)
//        └── BoundControl<T, Parameter>   (adds typed binding +
//        value/set_value)
//             └── ParameterT<T>           (adds typed observers)
//                  └── FloatParameter     (abstract; adds init/min/max/step,
//                  │    │                  snap/clamp, normalized mapping)
//                  │    ├── LinearParameter
//                  │    ├── DecibelParameter
//                  │    └── FrequencyParameter
//                  └── ChoiceParameter    (adds choices list, index I/O)
//
// Parameter exposes only what the UI/Rack layer needs without knowing T:
// normalized I/O, formatted display, reset-to-default, and an untyped change
// listener. Typed access (value(), typed observers) lives on ParameterT<T>;
// numeric-range semantics live on FloatParameter.

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

// ---------------------------------------------------------------------------
// Parameter: non-template abstract base. This is what generic UI and Rack
// code holds pointers to.
// ---------------------------------------------------------------------------
class Parameter : public Control {
public:
    using ChangeListener = std::function<void()>;

    Parameter(std::string label, ControlMeta meta)
        : Control(std::move(label), std::move(meta)) {}

    ~Parameter() override = default;

    Parameter(const Parameter &) = delete;
    Parameter &operator=(const Parameter &) = delete;
    Parameter(Parameter &&) = delete;
    Parameter &operator=(Parameter &&) = delete;

    // Normalized [0, 1] view for ImGui and automation.
    virtual fy_real GetNormalized() const = 0;
    virtual void SetNormalized(fy_real pos) = 0;

    // Human-readable value with units, e.g. "-6.0 dB", "Sine".
    virtual std::string Format() const = 0;

    // Restore the parameter to its default/initial value.
    virtual void ResetToInit() = 0;

    bool IsKnob() const { return meta_.IsKnob(); }

    // Untyped subscription for UI refresh etc. Typed subscribers should use
    // ParameterT<T>::AddObserver instead.
    void AddChangeListener(ChangeListener cb) {
        listeners_.push_back(std::move(cb));
    }

protected:
    void NotifyChanged() {
        for (auto &cb : listeners_)
            cb();
    }

private:
    std::vector<ChangeListener> listeners_;
};

// ---------------------------------------------------------------------------
// ParameterT<T>: adds typed binding (via BoundControl) and typed observers.
// Does NOT assume a numeric range -- ChoiceParameter uses this too.
// ---------------------------------------------------------------------------
template <typename T, typename TBase = Parameter>
class ParameterT : public BoundControl<T, TBase> {
    static_assert(std::is_base_of_v<Parameter, TBase>,
                  "ParameterT's TBase must derive from Parameter");

public:
    using Base = BoundControl<T, TBase>;
    using BindingPtr = typename Base::BindingPtr;
    using Observer = std::function<void(T)>;

    ParameterT(std::string label, ControlMeta meta, BindingPtr binding)
        : Base(std::move(label), std::move(meta), std::move(binding)) {}

    // Typed subscription -- fires after any programmatic value change.
    void AddObserver(Observer cb) { observers_.push_back(std::move(cb)); }

protected:
    // Subclasses call this from their overridden set_value() after the value
    // has been written to the binding.
    void NotifyObservers() {
        const T v = this->value();
        for (auto &cb : observers_)
            cb(v);
        this->NotifyChanged();
    }

private:
    std::vector<Observer> observers_;
};

} // namespace augr
