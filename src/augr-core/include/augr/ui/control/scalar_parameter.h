// scalar_parameter.h
#pragma once
#include <algorithm>
#include <cmath>
#include <augr/ui/control/parameter.h>

namespace augr {

template <typename T>
class ScalarParameterT : public ParameterT<T> {
public:
    using BindingPtr = typename ParameterT<T>::BindingPtr;

    ScalarParameterT(std::string label, ControlMeta meta, BindingPtr binding,
                     T init, T min, T max, T step)
        : ParameterT<T>(std::move(label), std::move(meta), std::move(binding))
        , init_(init), min_(min), max_(max), step_(step) {}

    void set_value(const T &value) override {
        if (this->binding_) {
            auto snapped = SnapAndClamp(value);
            this->binding_->set(snapped);
            this->on_change(snapped);
        }
    }

    void ResetToInit() override { set_value(init_); }

    T init() const { return init_; }
    T min()  const { return min_; }
    T max()  const { return max_; }
    T step() const { return step_; }

protected:
    T SnapAndClamp(T v) const {
        if (step_ > T{0}) {
            // Round to nearest step from min.
            // For int, std::round isn't needed but the expression is valid.
            v = min_ + static_cast<T>(std::round(
                    static_cast<double>(v - min_) / static_cast<double>(step_)
                )) * step_;
        }
        return std::clamp(v, min_, max_);
    }

private:
    T init_, min_, max_, step_;
};

} // namespace augr