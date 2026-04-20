#ifndef AUGR_CORE_CONTROL_CONTROL_H_
#define AUGR_CORE_CONTROL_CONTROL_H_

#include <memory>
#include <string>
#include <utility>

#include <augr/core/binding.h>
#include <augr/core/config.h>
#include <augr/core/model.h>
#include <augr/core/ui/control/control_meta.h>

namespace augr {

class Control : public Model {
public:
    Control() = default;
    Control(std::string label, ControlMeta meta = {})
        : label_(std::move(label)), meta_(std::move(meta)),
          unit_(meta_.Unit()) {}

    virtual ~Control() = default;

    // Accessors
    const std::string &label() const { return label_; }
    const ControlMeta &meta() const { return meta_; }
    ControlUnit unit() const { return unit_; }

    // Data members.
    std::string label_;
    ControlMeta meta_;
    ControlUnit unit_ = ControlUnit::kNone;

    REFLECT_ENABLE(Model)
};

template <typename T, typename TBase = Control> class BoundControl : public TBase {
public:
    using BindingType = BindingT<T>;
    using BindingPtr = std::shared_ptr<BindingType>;

    BoundControl() = default;

    explicit BoundControl(std::string label, ControlMeta meta = {},
                          BindingPtr binding = nullptr)
        : TBase(std::move(label), std::move(meta)),
          binding_(std::move(binding)) {}

    T value() const { return binding_ ? binding_->get() : T{}; }

    // Virtual so subclasses (e.g. Parameter) can add snap/clamp/observer logic.
    virtual void set_value(const T &value) {
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
using IntControl = BoundControl<int>;
using BoolControl = BoundControl<bool>;

} // namespace augr

#endif // AUGR_CORE_CONTROL_CONTROL_H_