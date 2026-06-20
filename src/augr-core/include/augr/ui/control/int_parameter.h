#pragma once

#include <format>
#include <memory>
#include <string>
#include <utility>
#include <cmath>

#include <augr/ui/control/scalar_parameter.h>

namespace augr {

class IntParameter : public ScalarParameterT<int> {
public:
    using BindingPtr = ScalarParameterT<int>::BindingPtr;

    IntParameter(std::string label, ControlMeta meta, BindingPtr binding,
                 int init, int min, int max, int step = 1,
                 std::string suffix = "")
        : ScalarParameterT<int>(std::move(label), std::move(meta),
                                std::move(binding), init, min, max, step)
        , suffix_(std::move(suffix)) {}

    // Normalized is linear over [min, max] for widgets that need a 0-1 float.
    fy_real GetNormalized() const override {
        if (max() == min()) return fy_real{0};
        return static_cast<fy_real>(value() - min()) /
               static_cast<fy_real>(max() - min());
    }

    void SetNormalized(fy_real pos) override {
        set_value(min() + static_cast<int>(
            std::round(pos * static_cast<fy_real>(max() - min()))));
    }

    std::string Format() const override {
        return std::format("{}{}", value(), suffix_);
    }

    // Factory: mirrors FloatParameter::Make, unit-aware suffix selection.
    static std::unique_ptr<IntParameter>
    Make(std::string label, ControlMeta meta, BindingPtr binding,
         int init, int min, int max, int step = 1);

private:
    std::string suffix_;
};

inline std::unique_ptr<IntParameter>
IntParameter::Make(std::string label, ControlMeta meta,
                   IntParameter::BindingPtr binding,
                   int init, int min, int max, int step) {
    std::string suffix;
    switch (meta.Unit()) {
    case ControlUnit::kMilliseconds: suffix = " ms";  break;
    case ControlUnit::kSeconds:      suffix = " s";   break;
    case ControlUnit::kPercent:      suffix = "%";    break;
    case ControlUnit::kSemitones:    suffix = " st";  break;
    case ControlUnit::kBpm:          suffix = " bpm"; break;
    default:                         suffix = "";     break;
    }
    return std::make_unique<IntParameter>(
        std::move(label), std::move(meta), std::move(binding),
        init, min, max, step, std::move(suffix));
}

} // namespace augr