#pragma once

namespace augr {

class Slot {
public:
    Slot() = default;
    virtual ~Slot() = default;
};

template <typename T>
class SlotT : public Slot {
public:
    SlotT() = default;
    explicit SlotT(T value) : value_(value) {}

    T Read() const { return value_; }
    void Write(T value) { value_ = value; }

    T value_{};
};

} // namespace augr