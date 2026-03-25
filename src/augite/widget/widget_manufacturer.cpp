//#include <rttr/type>
#include <augr/core/reflect.h>

#include "widget_manufacturer.h"
#include "widget.h"

namespace augr {

void WidgetManufacturer::AddFactory(WidgetFactory& factory) {
  factories_.push_back(&factory);
  factory_map_[factory.GetKey()] = &factory;
}

WidgetFactory* WidgetManufacturer::GetFactory(std::type_index& key) {
  return FindFactory(key);
}

WidgetFactory* WidgetManufacturer::FindFactory(const std::type_index& t) {
  // exact match
  if (const auto it = factory_map_.find(t); it != factory_map_.end())
    return it->second;

  // search bases (depth-first)
  for (const auto& b : ::reflect::Registry::instance().bases_of(t)) {
    if (auto* f = FindFactory(b))
      return f;
  }
  return nullptr;
}

} // namespace augr