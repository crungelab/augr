#include <iostream>

#include <augr/core/archiver_manufacturer.h>
#include <augr/core/subject.h>

#include <augr/core/archiver.h>
#include <augr/core/archiver_factory.h>

namespace augr {

void ArchiverManufacturer::AddFactory(ArchiverFactory &factory) {
    factories_.push_back(&factory);
    factory_type_map_[factory.SubjectType()] = &factory;
    factory_name_map_[factory.type_name()] = &factory;
}

ArchiverFactory *
ArchiverManufacturer::FindFactory(const std::type_index &type) const {
    auto it = factory_type_map_.find(type);
    return it == factory_type_map_.end() ? nullptr : it->second;
}

ArchiverFactory *
ArchiverManufacturer::FindFactory(const std::string &type_name) const {
    auto it = factory_name_map_.find(type_name);
    return it == factory_name_map_.end() ? nullptr : it->second;
}

Archiver *ArchiverManufacturer::MakeArchiver(Subject &subject) const {
    auto factory = FindFactory(std::type_index(typeid(subject)));
    return factory ? factory->Produce(subject) : nullptr;
}

Archiver *ArchiverManufacturer::MakeArchiver(const std::string &type_name,
                                             Subject &subject) const {
    auto factory = FindFactory(type_name);
    return factory ? factory->Produce(subject) : nullptr;
}

void ArchiverManufacturer::Serialize(Archive &archive, Subject &subject) const {
    auto *factory = FindFactory(std::type_index(typeid(subject)));
    if (!factory) { /* error */
        return;
    }
    Archiver *archiver = factory->Produce(subject);
    if (archiver) {
        archiver->Save(archive);
        delete archiver;
    }
}

void ArchiverManufacturer::Deserialize(Archive &archive,
                                       Subject &subject) const {
    const auto &j = archive.json();

    if (!j.contains("type")) {
        std::cerr << "Deserialize: JSON missing 'type' field\n";
        return;
    }
    if (!j["type"].is_string()) {
        std::cerr << "Deserialize: 'type' field is not a string\n";
        return;
    }

    std::string type_name = j["type"].get<std::string>();

    ArchiverFactory *factory = FindFactory(type_name);
    if (!factory) {
        std::cerr << "Deserialize: no archiver factory registered for type '"
                  << type_name << "'\n";
        return;
    }

    std::unique_ptr<Archiver> archiver(factory->Produce(subject));
    if (!archiver) {
        std::cerr << "Deserialize: factory produced null archiver for type '"
                  << type_name << "'\n";
        return;
    }

    archiver->Load(archive);
}

} // namespace augr