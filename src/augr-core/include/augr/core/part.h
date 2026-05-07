#pragma once

// #include <rttr/type>
#include "reflect.h"

namespace augr {

class Part {
public:
    virtual ~Part() = default;
    Part() : owner_(nullptr), id_(instanceCounter_++) {}
    virtual void Create(Part *owner = nullptr) {
        owner_ = owner;
    }
    virtual void Destroy() {}
    // Accessors
    Part &owner() { return *owner_; }
    //
    // Data members
    Part *owner_ = nullptr;
    static int instanceCounter_;
    int id_ = 0;
    REFLECT_ENABLE()
};

} // namespace augr