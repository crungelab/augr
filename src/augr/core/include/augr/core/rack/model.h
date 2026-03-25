#pragma once

#include <string>
#include <utility>
#include <vector>

#include <augr/core/part.h>

namespace augr {

class Model : public Part {
public:
    Model() = default;
    void AddChild(Model &model) { children_.push_back(&model); }
    // Accessors
    [[nodiscard]] Model &parent() const {
        return *dynamic_cast<Model *>(owner_);
    }
    // Data members
    std::vector<Model *> children_;

    REFLECT_ENABLE(Part)
};

template <typename T = Model> class ModelT : T {
    void AddChild(T &model) { children_.push_back(&model); }
    // Data members
    std::vector<T *> children_;
};

// Factory
class ModelFactory {
public:
    virtual ~ModelFactory() = default;
    ModelFactory(std::string name, std::string category)
        : name_(std::move(name)), category_(std::move(category)) {}
    virtual Model *Produce(Model &model) = 0;
    virtual std::type_index GetKey() = 0;
    // Data members
    std::string name_;
    std::string category_;
};

template <typename T> class ModelFactoryT final : public ModelFactory {
public:
    // ModelFactoryT() {}
    using ModelFactory::ModelFactory;
    static T &Make(Model &parent) {
        T &model = *new T();
        model.Create(parent);
        return model;
    }
    Model *Produce(Model &parent) override { return &Make(parent); }
    std::type_index GetKey() override { return std::type_index(typeid(T)); }
    // Data members
};

#define DEFINE_MODEL_FACTORY(T, NAME, CATEGORY)                                \
    ModelFactoryT<T> T##Factory(NAME, CATEGORY);                               \
    ModelFactory *Get##T##Factory() { return &T##Factory; }

} // namespace augr