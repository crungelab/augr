#pragma once

#include <map>
#include <string>
#include <typeindex>
#include <vector>

#include <augr/manufacturer.h>

namespace augr {

class ModelWidgetFactory;

class ModelWidgetManufacturer : public Manufacturer<ModelWidgetFactory> {
public:
    static ModelWidgetManufacturer &singleton() noexcept {
        static ModelWidgetManufacturer *self = new ModelWidgetManufacturer();
        return *self;
    }
};

#define REGISTER_MODEL_WIDGET_FACTORY(T)                                       \
    extern ModelWidgetFactory *Get##T##Factory();                              \
    ModelWidgetManufacturer::singleton().AddFactory(*Get##T##Factory());

} // namespace augr