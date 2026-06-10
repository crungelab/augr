#pragma once

#include <map>
#include <string>
#include <typeindex>
#include <vector>

#include "model_factory.h"
#include "manufacturer.h"

namespace augr {

class ModelManufacturer : public Manufacturer<ModelFactory> {
public:
    static ModelManufacturer &singleton() noexcept {
        static auto *self = new ModelManufacturer();
        return *self;
    }
};

#define REGISTER_MODEL_FACTORY(T)                                              \
    extern ModelFactory *Get##T##Factory();                                    \
    ModelManufacturer::singleton().AddFactory(*Get##T##Factory());

} // namespace augr