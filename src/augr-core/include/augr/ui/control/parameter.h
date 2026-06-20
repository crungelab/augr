#pragma once

#include <algorithm>
#include <cassert>
#include <cmath>
#include <format>
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <augr/binding.h>
#include <augr/ui/control/control.h>
#include <augr/ui/control/control_meta.h>

namespace augr {

// ---------------------------------------------------------------------------
// Parameter: non-template abstract base. This is what generic UI and Rack
// code holds pointers to.
// ---------------------------------------------------------------------------
class Parameter : public Control {
public:
    Parameter(const std::string &label, ControlMeta meta)
        : Control(label, std::move(meta)) {}

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

    virtual void LinkTo(Parameter &master) = 0;
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

    ParameterT(std::string label, ControlMeta meta, BindingPtr binding)
        : Base(std::move(label), std::move(meta), std::move(binding)) {}

    void LinkTo(Parameter &master) override {
        auto *typed = dynamic_cast<ParameterT<T, TBase> *>(&master);
        assert(typed);
        Link(*typed);
    }

    void Link(ParameterT<T, TBase> &master) {
        link_connection_ = master.on_change.connect([this](T v) {
            if (this->binding_)
                this->binding_->set(v);
        });
    }

private:
    sigslot::scoped_connection link_connection_;
};

} // namespace augr
