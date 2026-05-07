#pragma once

#include <map>
#include <string>
#include <typeindex>
#include <vector>

#include "model_factory.h"

namespace augr {

class ModelManufacturer {
public:
    static ModelManufacturer &singleton() noexcept {
        static auto *self = new ModelManufacturer();
        return *self;
    }
    void AddFactory(ModelFactory &factory);
    ModelFactory *GetFactory(std::type_index &key);
    ModelFactory *FindFactory(const std::type_index &type);

    // Lookup by string type tag — for deserialization, where the JSON
    // "type" field gives us a name and we need the matching factory.
    ModelFactory *FindFactory(const std::string &type_name);

    // Data members
    std::vector<ModelFactory *> factories_;
    std::map<std::type_index, ModelFactory *> factory_type_map_;
    std::map<std::string, ModelFactory *> factory_name_map_;
};

#define REGISTER_MODEL_FACTORY(T)                                              \
    extern ModelFactory *Get##T##Factory();                                    \
    ModelManufacturer::singleton().AddFactory(*Get##T##Factory());

} // namespace augr