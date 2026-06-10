#pragma once

#include <map>
#include <string>
#include <typeindex>
#include <vector>

#include "../widget/widget_manufacturer.h"

namespace augr {

class InspectorManufacturer : public ModelWidgetManufacturer {
public:
    static InspectorManufacturer &singleton() noexcept {
        static InspectorManufacturer *self = new InspectorManufacturer();
        return *self;
    }
};

#define REGISTER_INSPECTOR_FACTORY(T)                                       \
    extern ModelWidgetFactory *Get##T##Factory();                              \
    InspectorManufacturer::singleton().AddFactory(*Get##T##Factory());

} // namespace augr