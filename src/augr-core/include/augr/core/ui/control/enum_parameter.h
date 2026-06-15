#pragma once

// Enum parameters.
//
// Two-layer design so controls can be generic:
//
//   EnumParameter         -- non-template abstract. Index-based view of the
//                            enum, sufficient for dropdown/radio UIs.
//   EnumParameterT<T>     -- concrete, typed. Holds the (T, label) choices
//                            list and the typed binding.
//
// UIs hold EnumParameter*. Host/audio code that needs the typed value holds
// EnumParameterT<MyEnum>* (or calls value() on it directly).

#include <augr/core/ui/control/parameter.h>

namespace augr {

// ---------------------------------------------------------------------------
// EnumParameter: non-template base. The UI-facing view.
// ---------------------------------------------------------------------------
class EnumParameter : public Parameter {
public:
    EnumParameter(std::string label, ControlMeta meta)
        : Parameter(std::move(label), std::move(meta)) {}

    // Number of choices.
    virtual std::size_t size() const = 0;

    // Label for the i-th choice. Precondition: 0 <= i < size().
    virtual const std::string &LabelAt(int i) const = 0;

    // Current selection as an index into the choices list.
    virtual int CurrentIndex() const = 0;

    // Write by index. Out-of-range indices are ignored.
    virtual void SetIndex(int i) = 0;

    // Label of the current selection. Default implementation in terms of
    // CurrentIndex()/LabelAt(). Subclasses can override if they have a
    // cheaper path.
    std::string Format() const override {
        const int i = CurrentIndex();
        return (i >= 0 && i < static_cast<int>(size())) ? LabelAt(i)
                                                        : std::string{};
    }

    fy_real GetNormalized() const override {
        if (size() <= 1)
            return fy_real{0};
        return static_cast<fy_real>(CurrentIndex()) /
               static_cast<fy_real>(size() - 1);
    }

    void SetNormalized(fy_real pos) override {
        const int n = static_cast<int>(size());
        if (n <= 0)
            return;
        SetIndex(
            std::clamp(static_cast<int>(std::round(pos * (n - 1))), 0, n - 1));
    }
};

// ---------------------------------------------------------------------------
// EnumParameterT<T>: typed concrete class.
// ---------------------------------------------------------------------------
template <typename T>
class EnumParameterT : public ParameterT<T, EnumParameter> {
    static_assert(std::is_enum_v<T>,
                  "EnumParameterT<T> requires T to be an enum type");

public:
    using Base = ParameterT<T, EnumParameter>;
    using BindingPtr = typename Base::BindingPtr;

    struct Choice {
        T value;
        std::string label;
    };

    EnumParameterT(std::string label, ControlMeta meta, BindingPtr binding,
                   std::vector<Choice> choices, T init)
        : Base(std::move(label), std::move(meta), std::move(binding)),
          choices_(std::move(choices)), init_(init) {
        assert(!choices_.empty());
        assert(IndexOf(init_).has_value() &&
               "EnumParameterT init value must appear in choices");
    }

    // ---- EnumParameter overrides (index-based) -----------------------------

    std::size_t size() const override { return choices_.size(); }

    const std::string &LabelAt(int i) const override {
        assert(i >= 0 && i < static_cast<int>(choices_.size()));
        return choices_[i].label;
    }

    int CurrentIndex() const override {
        return IndexOf(this->value()).value_or(0);
    }

    void SetIndex(int i) override {
        if (i < 0 || i >= static_cast<int>(choices_.size()))
            return;
        set_value(choices_[i].value);
    }

    // ---- Typed interface ---------------------------------------------------

    // Typed write. Silently drops values not present in the choices list.
    void set_value(const T &value) override {
        if (!IndexOf(value).has_value())
            return;
        if (this->binding_) {
            this->binding_->set(value);
            this->on_change(value);
        }
    }

    void ResetToInit() override { set_value(init_); }

    const std::vector<Choice> &choices() const { return choices_; }
    T init() const { return init_; }

    std::optional<int> IndexOf(T v) const {
        for (std::size_t i = 0; i < choices_.size(); ++i) {
            if (choices_[i].value == v)
                return static_cast<int>(i);
        }
        return std::nullopt;
    }

private:
    std::vector<Choice> choices_;
    T init_;
};

} // namespace augr
