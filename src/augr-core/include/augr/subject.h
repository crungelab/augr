#pragma once

// #include <rttr/type>
#include "reflect.h"

namespace augr {

class Subject {
public:
    virtual ~Subject() = default;
    // Accessors
    // Data members
    REFLECT_ENABLE()
};

} // namespace augr