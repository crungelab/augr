#pragma once

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include "model.h"

namespace augr {

class ModelFactory {
public:
    virtual ~ModelFactory() = default;
    ModelFactory(std::string name, std::string category)
        : name_(std::move(name)), category_(std::move(category)) {}
    virtual Model::Ptr Produce(Model::Ptr parent = nullptr,
                               CreateMode mode = CreateMode::Fresh) = 0;
    virtual std::type_index GetKey() = 0;
    // Accessors
    const std::string &name() const { return name_; }
    const std::string &category() const { return category_; }
    // Data members
    std::string name_;
    std::string category_;
};

template <typename T> class ModelFactoryT : public ModelFactory {
public:
    using ModelFactory::ModelFactory;

    static std::shared_ptr<T> Make(Model::Ptr parent = nullptr,
                                   CreateMode mode = CreateMode::Fresh) {
        return Model::Make<T>(parent, mode);
    }

    Model::Ptr Produce(Model::Ptr parent = nullptr,
                       CreateMode mode = CreateMode::Fresh) override {
        return Make(parent, mode);
    }

    std::type_index GetKey() override { return std::type_index(typeid(T)); }
};

#define DEFINE_MODEL_FACTORY(T, NAME, CATEGORY)                                \
    ModelFactoryT<T> T##Factory(NAME, CATEGORY);                               \
    ModelFactory *Get##T##Factory() { return &T##Factory; }

} // namespace augr