#pragma once

#include <string>
#include <utility>
#include <vector>
#include <algorithm>

#include "model.h"

namespace augr {

class ModelFactory {
public:
    virtual ~ModelFactory() = default;
    ModelFactory(std::string name, std::string category)
        : name_(std::move(name)), category_(std::move(category)) {}
    virtual Model *Produce(Model *parent = nullptr, CreateMode mode = CreateMode::Fresh) = 0;
    virtual std::type_index GetKey() = 0;
    // Data members
    std::string name_;
    std::string category_;
};

template <typename T> class ModelFactoryT : public ModelFactory {
public:
    using ModelFactory::ModelFactory;

    static T *Make(Model *parent = nullptr, CreateMode mode = CreateMode::Fresh) {
        return &Model::Make<T>(parent, mode);
    }

    Model *Produce(Model *parent = nullptr, CreateMode mode = CreateMode::Fresh) override { 
        return Make(parent, mode); 
    }

    std::type_index GetKey() override { return std::type_index(typeid(T)); }
};

/*
template <typename T> class ModelFactoryT : public ModelFactory {
public:
    using ModelFactory::ModelFactory;

    static T *Make(Model *parent = nullptr) {
        return &Model::Make<T>(parent);
    }

    Model *Produce(Model *parent = nullptr, CreateMode mode = CreateMode::Fresh) override { 
        auto model = Make(parent); 
        if (mode == CreateMode::Fresh) {
            model->OnFresh();
        } else {
            model->OnLoaded();
        }
        return model;
    }
    std::type_index GetKey() override { return std::type_index(typeid(T)); }
    // Data members
};
*/

#define DEFINE_MODEL_FACTORY(T, NAME, CATEGORY)                                \
    ModelFactoryT<T> T##Factory(NAME, CATEGORY);                               \
    ModelFactory *Get##T##Factory() { return &T##Factory; }

} // namespace augr