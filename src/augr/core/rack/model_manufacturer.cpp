//#include <rttr/type>
#include <augr/core/reflect.h>
#include <augr/core/rack/model_manufacturer.h>
#include <augr/core/rack/model.h>

namespace augr {

void ModelManufacturer::AddFactory(ModelFactory& factory) {
  factories_.push_back(&factory);
  factory_map_[factory.GetKey()] = &factory;
}

ModelFactory* ModelManufacturer::GetFactory(std::type_index& key) {
  return FindFactory(key);
}

ModelFactory* ModelManufacturer::FindFactory(const std::type_index& t) {
  // exact match
  if (auto it = factory_map_.find(t); it != factory_map_.end())
    return it->second;

  // search bases (depth-first)
  for (const auto& b : ::reflect::Registry::instance().bases_of(t)) {
    if (auto* f = FindFactory(b))
      return f;
  }
  return nullptr;
}

} // namespace augr