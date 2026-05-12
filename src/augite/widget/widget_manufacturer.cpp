// #include <rttr/type>
#include <augr/core/reflect.h>

#include "widget.h"
#include "widget_manufacturer.h"

namespace augr {

void ModelWidgetManufacturer::AddFactory(ModelWidgetFactory &factory) {
    factories_.push_back(&factory);
    factory_type_map_[factory.GetKey()] = &factory;
}

ModelWidgetFactory *ModelWidgetManufacturer::GetFactory(std::type_index &key) {
    return FindFactory(key);
}

ModelWidgetFactory *
ModelWidgetManufacturer::FindFactory(const std::type_index &t) {
    // exact match
    if (const auto it = factory_type_map_.find(t);
        it != factory_type_map_.end())
        return it->second;

    // search bases (depth-first)
    for (const auto &b : ::reflect::Registry::singleton().bases_of(t)) {
        if (auto *f = FindFactory(b))
            return f;
    }
    return nullptr;
}

} // namespace augr