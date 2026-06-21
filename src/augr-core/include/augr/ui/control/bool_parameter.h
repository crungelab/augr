#pragma once

#include <memory>
#include <string>
#include <utility>

#include <augr/ui/control/parameter.h>

namespace augr {

class BoolParameter : public ParameterT<bool> {
public:
    using BindingPtr = ParameterT<bool>::BindingPtr;

    BoolParameter(std::string label, ControlMeta meta, BindingPtr binding,
                  bool init)
        : ParameterT<bool>(std::move(label), std::move(meta),
                           std::move(binding))
        , init_(init) {}

    void set_value(const bool &value) override {
        if (this->binding_) {
            this->binding_->set(value);
            this->on_change(value);
        }
    }

    void ResetToInit() override { set_value(init_); }

    // Normalized: false = 0.0, true = 1.0.
    fy_real GetNormalized() const override {
        return value() ? fy_real{1} : fy_real{0};
    }

    void SetNormalized(fy_real pos) override {
        set_value(pos >= fy_real{0.5});
    }

    std::string Format() const override {
        return value() ? "On" : "Off";
    }

    bool init() const { return init_; }

    static std::unique_ptr<BoolParameter>
    Make(std::string label, ControlMeta meta, BindingPtr binding, bool init);

private:
    bool init_;
};

inline std::unique_ptr<BoolParameter>
BoolParameter::Make(std::string label, ControlMeta meta,
                    BoolParameter::BindingPtr binding, bool init) {
    return std::make_unique<BoolParameter>(
        std::move(label), std::move(meta), std::move(binding), init);
}

} // namespace augr