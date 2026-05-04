#include <augr/core/model.h>
#include <augr/core/archiver_manufacturer.h>

#include <augr/core/archiver.h>
#include <augr/core/archiver_factory.h>

namespace augr {

void ArchiverManufacturer::AddFactory(ArchiverFactory &factory) {
    factories_.push_back(&factory);
    by_type_[factory.ModelType()] = &factory;
    by_name_[factory.type_name()] = &factory;
}

ArchiverFactory *ArchiverManufacturer::FindFactory(
    const std::type_index &type) const {
    auto it = by_type_.find(type);
    return it == by_type_.end() ? nullptr : it->second;
}

ArchiverFactory *ArchiverManufacturer::FindFactory(
    const std::string &type_name) const {
    auto it = by_name_.find(type_name);
    return it == by_name_.end() ? nullptr : it->second;
}

Archiver* ArchiverManufacturer::MakeArchiver(
    Model &model) const {
    auto factory = FindFactory(std::type_index(typeid(model)));
    return factory ? factory->Produce(model) : nullptr;
}

Archiver* ArchiverManufacturer::MakeArchiver(
    const std::string &type_name, Model &model) const {
    auto factory = FindFactory(type_name);
    return factory ? factory->Produce(model) : nullptr;
}

void ArchiverManufacturer::Serialize(Archive &archive, Model &model) const {
    auto *factory = FindFactory(std::type_index(typeid(model)));
    if (!factory) { /* error */ return; }
    Archiver *archiver = factory->Produce(model);
    if (archiver) {
        archiver->Save(archive);
        delete archiver;
    }
}

void ArchiverManufacturer::Deserialize(Archive &archive, Model &model) const {
    // mirror for Load
}

} // namespace augr