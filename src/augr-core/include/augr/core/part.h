#pragma once

// #include <rttr/type>
#include "reflect.h"

namespace augr {

class Part {
public:
    virtual ~Part() = default;
    Part() : owner_(nullptr), id_(instanceCounter_++) {}
    virtual bool Create(Part &owner) {
        owner_ = &owner;
        return true;
    }
    virtual bool Create() {
        owner_ = nullptr;
        return true;
    }
    virtual void Destroy() {}
    // Accessors
    Part &owner() { return *owner_; }
    //
    // Data members
    Part *owner_;
    static int instanceCounter_;
    int id_ = 0;
    REFLECT_ENABLE()
};

} // namespace augr