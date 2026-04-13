#pragma once

#include <memory>
#include <string>
#include <vector>

#include <augr/core/config.h>
#include <augr/core/model.h>
#include <augr/core/binding.h>

#include <augr/core/control/parameter_meta.h>

namespace augr {

class Control : public Model {
public:
    Control() = default;
    Control(std::string label, ParameterMeta meta = {})
        : label_(std::move(label)), meta_(std::move(meta)), unit_(meta_.Unit()) {}
    // Data members
    std::string label_;
    ParameterMeta meta_;
    ParameterUnit unit_ = ParameterUnit::kNone;

    REFLECT_ENABLE(Model)
};

template <typename T> class BoundControl : public Control {
public:
    using BindingType = BindingT<T>;
    using BindingPtr = std::shared_ptr<BindingType>;

    BoundControl() = default;

    explicit BoundControl(std::string label, BindingPtr binding = nullptr, ParameterMeta meta = {})
        : Control(std::move(label), std::move(meta)), binding_(std::move(binding)) {}

    T value() const { return binding_ ? binding_->get() : T{}; }

    void set_value(const T &value) {
        if (binding_) {
            binding_->set(value);
        }
    }

    BindingType *binding() { return binding_.get(); }
    const BindingType *binding() const { return binding_.get(); }

    void set_binding(BindingPtr binding) { binding_ = std::move(binding); }

    bool has_binding() const { return static_cast<bool>(binding_); }

protected:
    BindingPtr binding_;
};

using FloatControl = BoundControl<fy_real>;
using BoolControl = BoundControl<bool>;

} // namespace augr