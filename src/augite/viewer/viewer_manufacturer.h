#pragma once

#include <map>
#include <string>
#include <typeindex>
#include <vector>

#include <augr/core/manufacturer.h>

namespace augr {

class ViewerFactory;

class ViewerManufacturer : public Manufacturer<ViewerFactory> {
public:
    static ViewerManufacturer &singleton() noexcept {
        static ViewerManufacturer *self = new ViewerManufacturer();
        return *self;
    }
};

#define REGISTER_VIEWER_FACTORY(T)                                              \
    extern ViewerFactory *Get##T##Factory();                                    \
    ViewerManufacturer::singleton().AddFactory(*Get##T##Factory());

} // namespace augr