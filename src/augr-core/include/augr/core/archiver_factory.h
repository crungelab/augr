#pragma once

#include <memory>
#include <string>
#include <typeindex>
#include <utility>

namespace augr {

class Archiver;
class Subject;

// ArchiverFactory produces archiver instances for a specific Model type.
// One factory per Model subclass, registered with ArchiverManufacturer.
class ArchiverFactory {
public:
    virtual ~ArchiverFactory() = default;

    ArchiverFactory(std::string type_name) : type_name_(std::move(type_name)) {}

    // Produce a new archiver bound to the given model. The factory
    // constructs the archiver and calls Create(model) before returning.
    virtual Archiver *Produce(Subject &subject) = 0;

    virtual std::type_index SubjectType() const = 0;

    const std::string &type_name() const { return type_name_; }

private:
    std::string type_name_;
};

template <typename ArchiverT, typename SubjectT>
class ArchiverFactoryT final : public ArchiverFactory {
public:
    using ArchiverFactory::ArchiverFactory;

    Archiver *Produce(Subject &subject) override {
        ArchiverT *archiver = new ArchiverT();
        archiver->Create(*this, subject);
        return archiver;
    }

    std::type_index SubjectType() const override { return typeid(SubjectT); }
};

#define DEFINE_ARCHIVER_FACTORY(ArchiverT, SubjectT, NAME)                       \
    ArchiverFactoryT<ArchiverT, SubjectT> ArchiverT##Factory(NAME);              \
    ArchiverFactory *Get##ArchiverT##Factory() { return &ArchiverT##Factory; }

} // namespace augr