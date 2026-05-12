#pragma once

#include <map>
#include <string>
#include <typeindex>
#include <vector>

namespace augr {

class ModelWidgetFactory;

class ModelWidgetManufacturer {
public:
    static ModelWidgetManufacturer &singleton() noexcept {
        static ModelWidgetManufacturer *self = new ModelWidgetManufacturer();
        return *self;
    }
    void AddFactory(ModelWidgetFactory &factory);
    ModelWidgetFactory *GetFactory(std::type_index &key);
    ModelWidgetFactory *FindFactory(const std::type_index &type);
    // Data members
    std::vector<ModelWidgetFactory *> factories_;
    std::map<std::type_index, ModelWidgetFactory *> factory_type_map_;
};

#define REGISTER_MODEL_WIDGET_FACTORY(T)                                       \
    extern ModelWidgetFactory *Get##T##Factory();                              \
    ModelWidgetManufacturer::singleton().AddFactory(*Get##T##Factory());

} // namespace augr