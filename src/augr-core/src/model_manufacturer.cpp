//#include <rttr/type>
#include <augr/core/reflect.h>
#include <augr/core/model_manufacturer.h>
#include <augr/core/model.h>

namespace augr {

/*
void ModelManufacturer::AddFactory(ModelFactory& factory) {
  factories_.push_back(&factory);
  factory_type_map_[factory.GetKey()] = &factory;
}
*/
void ModelManufacturer::AddFactory(ModelFactory &factory) {
    factories_.push_back(&factory);
    factory_type_map_[factory.GetKey()] = &factory;
    factory_name_map_[factory.name_] = &factory;
}

ModelFactory *ModelManufacturer::FindFactory(const std::string &type_name) {
    auto it = factory_name_map_.find(type_name);
    return it == factory_name_map_.end() ? nullptr : it->second;
}

ModelFactory* ModelManufacturer::GetFactory(std::type_index& key) {
  return FindFactory(key);
}

ModelFactory* ModelManufacturer::FindFactory(const std::type_index& t) {
  // exact match
  if (auto it = factory_type_map_.find(t); it != factory_type_map_.end())
    return it->second;

  // search bases (depth-first)
  for (const auto& b : ::reflect::Registry::singleton().bases_of(t)) {
    if (auto* f = FindFactory(b))
      return f;
  }
  return nullptr;
}

} // namespace augr