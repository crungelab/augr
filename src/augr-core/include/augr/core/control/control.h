#pragma once

#include <memory>
#include <string>
#include <vector>

#include <augr/core/config.h>
#include <augr/core/model.h>
#include <augr/core/property.h>

namespace augr {

class Control : public Model {
public:
    Control() = default;
    Control(std::string label)
        : label_(std::move(label)) {}
    // Data members
    std::string label_;

    REFLECT_ENABLE(Model)
};

template <typename T> class PropertyControl : public Control {
public:
    using PropertyType = PropertyT<T>;
    using PropertyPtr = std::shared_ptr<PropertyType>;

    PropertyControl() = default;

    explicit PropertyControl(std::string label, PropertyPtr property = nullptr)
        : Control(std::move(label)), property_(std::move(property)) {}

    T value() const { return property_ ? property_->get() : T{}; }

    void set_value(const T &value) {
        if (property_) {
            property_->set(value);
        }
    }

    PropertyType *property() { return property_.get(); }
    const PropertyType *property() const { return property_.get(); }

    void set_property(PropertyPtr property) { property_ = std::move(property); }

    bool has_property() const { return static_cast<bool>(property_); }

protected:
    PropertyPtr property_;
};

using FloatControl = PropertyControl<fy_real>;
using BoolControl = PropertyControl<bool>;

} // namespace augr