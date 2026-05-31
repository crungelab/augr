#pragma once

#include <string>
#include <utility>
#include <vector>
#include <algorithm>

#include "model.h"

namespace augr {

// Factory
class ModelFactory {
public:
    virtual ~ModelFactory() = default;
    ModelFactory(std::string name, std::string category)
        : name_(std::move(name)), category_(std::move(category)) {}
    virtual Model *Produce(Model *parent = nullptr) = 0;
    virtual std::type_index GetKey() = 0;
    // Data members
    std::string name_;
    std::string category_;
};

template <typename T> class ModelFactoryT : public ModelFactory {
public:
    using ModelFactory::ModelFactory;

    static T *Make(Model *parent = nullptr) {
        return &Model::Make<T>(parent);
    }

    /*
    static T *Make(Model *parent = nullptr) {
        T *model = new T();
        model->Create(parent);
        return model;
    }
    */
    Model *Produce(Model *parent = nullptr) override { return Make(parent); }
    std::type_index GetKey() override { return std::type_index(typeid(T)); }
    // Data members
};

#define DEFINE_MODEL_FACTORY(T, NAME, CATEGORY)                                \
    ModelFactoryT<T> T##Factory(NAME, CATEGORY);                               \
    ModelFactory *Get##T##Factory() { return &T##Factory; }

} // namespace augr