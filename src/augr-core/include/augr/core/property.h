#pragma once

#include <functional>
#include <string>
#include <typeinfo>
#include <utility>

#include "config.h"

class Property {
public:
    Property() = default;
    virtual ~Property() = default;

    virtual const std::type_info& value_type() const = 0;
};

template <typename T>
class PropertyT : public Property {
public:
    using ValueType = T;

    PropertyT() = default;
    ~PropertyT() override = default;

    const std::type_info& value_type() const override { return typeid(T); }

    virtual T get() const = 0;
    virtual void set(const T& value) = 0;
};

template <typename T>
class ValueProperty : public PropertyT<T> {
public:
    ValueProperty() = default;

    explicit ValueProperty(T value) : value_(std::move(value)) {}

    T get() const override { return value_; }

    void set(const T& value) override { value_ = value; }

private:
    T value_{};
};

template <typename T>
class CallbackProperty : public PropertyT<T> {
public:
    using Getter = std::function<T()>;
    using Setter = std::function<void(const T&)>;

    CallbackProperty(Getter getter, Setter setter)
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

class ZoneProperty : public PropertyT<fy_real> {
public:
    ZoneProperty(fy_real* zone)
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