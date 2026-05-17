#include <augr/core/subject.h>
#include <augr/core/archiver_manufacturer.h>

#include <augr/core/archiver.h>
#include <augr/core/archiver_factory.h>

namespace augr {

void ArchiverManufacturer::AddFactory(ArchiverFactory &factory) {
    factories_.push_back(&factory);
    factory_type_map_[factory.SubjectType()] = &factory;
    factory_name_map_[factory.type_name()] = &factory;
}

ArchiverFactory *ArchiverManufacturer::FindFactory(
    const std::type_index &type) const {
    auto it = factory_type_map_.find(type);
    return it == factory_type_map_.end() ? nullptr : it->second;
}

ArchiverFactory *ArchiverManufacturer::FindFactory(
    const std::string &type_name) const {
    auto it = factory_name_map_.find(type_name);
    return it == factory_name_map_.end() ? nullptr : it->second;
}

Archiver* ArchiverManufacturer::MakeArchiver(
    Subject &subject) const {
    auto factory = FindFactory(std::type_index(typeid(subject)));
    return factory ? factory->Produce(subject) : nullptr;
}

Archiver* ArchiverManufacturer::MakeArchiver(
    const std::string &type_name, Subject &subject) const {
    auto factory = FindFactory(type_name);
    return factory ? factory->Produce(subject) : nullptr;
}

void ArchiverManufacturer::Serialize(Archive &archive, Subject &subject) const {
    auto *factory = FindFactory(std::type_index(typeid(subject)));
    if (!factory) { /* error */ return; }
    Archiver *archiver = factory->Produce(subject);
    if (archiver) {
        archiver->Save(archive);
        delete archiver;
    }
}

void ArchiverManufacturer::Deserialize(Archive &archive, Subject &subject) const {
    // mirror for Load
}

} // namespace augr