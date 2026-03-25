#pragma once

#include <map>
#include <string>
#include <typeindex>
#include <vector>

namespace augr {

class WidgetFactory;

class WidgetManufacturer {
public:
    static WidgetManufacturer &instance() noexcept {
        static WidgetManufacturer *self = new WidgetManufacturer();
        return *self;
    }
    void AddFactory(WidgetFactory &factory);
    WidgetFactory *GetFactory(std::type_index &key);
    WidgetFactory *FindFactory(const std::type_index &type);
    // Data members
    std::vector<WidgetFactory *> factories_;
    std::map<std::type_index, WidgetFactory *> factory_map_;
};

#define REGISTER_WIDGET_FACTORY(T)                                             \
    extern WidgetFactory *Get##T##Factory();                                   \
    WidgetManufacturer::instance().AddFactory(*Get##T##Factory());

} // namespace augr