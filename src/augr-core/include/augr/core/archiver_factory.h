#pragma once

#include <memory>
#include <string>
#include <typeindex>
#include <utility>

namespace augr {

class Archiver;
class Model;

// ArchiverFactory produces archiver instances for a specific Model type.
// One factory per Model subclass, registered with ArchiverManufacturer.
class ArchiverFactory {
public:
    virtual ~ArchiverFactory() = default;

    ArchiverFactory(std::string type_name) : type_name_(std::move(type_name)) {}

    // Produce a new archiver bound to the given model. The factory
    // constructs the archiver and calls Create(model) before returning.
    virtual Archiver *Produce(Model &model) = 0;

    virtual std::type_index ModelType() const = 0;

    const std::string &type_name() const { return type_name_; }

private:
    std::string type_name_;
};

template <typename ArchiverT, typename ModelT>
class ArchiverFactoryT final : public ArchiverFactory {
public:
    using ArchiverFactory::ArchiverFactory;

    Archiver *Produce(Model &model) override {
        ArchiverT *archiver = new ArchiverT();
        archiver->Create(*this, model);
        return archiver;
    }

    std::type_index ModelType() const override { return typeid(ModelT); }
};

#define DEFINE_ARCHIVER_FACTORY(ArchiverT, ModelT, NAME)                       \
    ArchiverFactoryT<ArchiverT, ModelT> ArchiverT##Factory(NAME);              \
    ArchiverFactory *Get##ArchiverT##Factory() { return &ArchiverT##Factory; }

} // namespace augr