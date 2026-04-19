#pragma once

#include <functional>
#include <string>
#include <typeinfo>
#include <utility>

#include "config.h"

namespace augr {

class Binding {
public:
    Binding() = default;
    virtual ~Binding() = default;

    virtual const std::type_info& value_type() const = 0;
};

template <typename T>
class BindingT : public Binding {
public:
    using ValueType = T;

    BindingT() = default;
    ~BindingT() override = default;

    const std::type_info& value_type() const override { return typeid(T); }

    virtual T get() const = 0;
    virtual void set(const T& value) = 0;
};

template <typename T>
class ValueBinding : public BindingT<T> {
public:
    ValueBinding() = default;

    explicit ValueBinding(T value) : value_(std::move(value)) {}

    T get() const override { return value_; }

    void set(const T& value) override { value_ = value; }

private:
    T value_{};
};

template <typename T>
class CallbackBinding : public BindingT<T> {
public:
    using Getter = std::function<T()>;
    using Setter = std::function<void(const T&)>;

    CallbackBinding(Getter getter, Setter setter)
        : getter_(std::move(getter)), setter_(std::move(setter)) {}

    T get() const override { return getter_ ? getter_() : T{}; }

    void set(const T& value) override {
        if (setter_) {
            setter_(value);
        }
    }

private:
    Getter getter_;
    Setter setter_;
};

class ZoneBinding : public BindingT<fy_real> {
public:
    ZoneBinding(fy_real* zone)
        : zone_(zone) {}

    fy_real get() const override { return zone_ ? *zone_ : fy_real{}; }

    void set(const fy_real& value) override {
        if (zone_) {
            *zone_ = value;
        }
    }

private:
    fy_real* zone_ = nullptr;
};

inline std::unique_ptr<BindingT<fy_real>> MakeZoneBinding(float *zone) {
    return std::make_unique<ZoneBinding>(zone);
}

} // namespace augr