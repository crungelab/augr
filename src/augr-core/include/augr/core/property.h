#pragma once

#include <functional>
#include <string>
#include <typeinfo>
#include <utility>

#include "config.h"

class Property {
public:
    Property() = default;
    explicit Property(std::string label) : label_(std::move(label)) {}
    virtual ~Property() = default;

    virtual const std::type_info& value_type() const = 0;

    std::string label_;
};

template <typename T>
class PropertyT : public Property {
public:
    using ValueType = T;

    PropertyT() = default;
    explicit PropertyT(std::string label) : Property(std::move(label)) {}
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

    ValueProperty(std::string label, T value = {})
        : PropertyT<T>(std::move(label)), value_(std::move(value)) {}

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

    CallbackProperty(std::string label, Getter getter, Setter setter)
        : PropertyT<T>(std::move(label)),
          getter_(std::move(getter)),
          setter_(std::move(setter)) {}

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
    ZoneProperty(std::string label, fy_real* zone)
        : PropertyT<fy_real>(std::move(label)), zone_(zone) {}

    fy_real get() const override { return zone_ ? *zone_ : fy_real{}; }

    void set(const fy_real& value) override {
        if (zone_) {
            *zone_ = value;
        }
    }

private:
    fy_real* zone_ = nullptr;
};