#pragma once

#include <map>
#include <string>
#include <typeindex>
#include <vector>

namespace augr {

class ModelFactory;

class ModelManufacturer {
public:
    static ModelManufacturer &singleton() noexcept {
        static auto *self = new ModelManufacturer();
        return *self;
    }
    void AddFactory(ModelFactory &factory);
    ModelFactory *GetFactory(std::type_index &key);
    ModelFactory *FindFactory(const std::type_index &type);
    // Data members
    std::vector<ModelFactory *> factories_;
    std::map<std::type_index, ModelFactory *> factory_map_;
};

#define REGISTER_MODEL_FACTORY(T)                                             \
    extern ModelFactory *Get##T##Factory();                                   \
    ModelManufacturer::singleton().AddFactory(*Get##T##Factory());

} // namespace augr