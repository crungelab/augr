#pragma once

namespace augr {

class Pin {
public:
    Pin() = default;
    virtual ~Pin() = default;
};

template <typename T>
class PinT : public Pin {
public:
    PinT() = default;
    explicit PinT(T value) : value_(value) {}

    T Read() const { return value_; }
    void Write(T value) { value_ = value; }

    T value_{};
};

} // namespace augr