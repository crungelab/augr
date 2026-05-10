#include <fstream>
#include <iostream>

#include <nlohmann/json.hpp>

#include <augr/rack/rack_doc.h>

#include <augr/core/archive.h>
#include <augr/core/archiver.h>
#include <augr/core/archiver_factory.h>
#include <augr/core/archiver_manufacturer.h>
#include <augr/core/model_manufacturer.h>

namespace augr {

// Serialize a rack to JSON and print it.
nlohmann::json SerializeRack(Rack &rack) {
    auto &manufacturer = ArchiverManufacturer::singleton();
    auto *factory = manufacturer.FindFactory(std::type_index(typeid(rack)));
    if (!factory) {
        std::cerr << "No archiver factory registered for type "
                  << typeid(rack).name() << "\n";
        return nlohmann::json();
    }

    auto *archiver = factory->Produce(rack);
    if (!archiver) {
        std::cerr << "Failed to construct archiver\n";
        return nlohmann::json();
    }

    nlohmann::json j;
    Archive archive(j);
    archiver->Save(archive);
    delete archiver;

    std::cout << j.dump(2) << "\n";
    return j;
}

Rack* NewRack(std::string type_name) {
    // Construct the rack as a root via its model factory.
    auto &model_manufacturer = ModelManufacturer::singleton();
    ModelFactory *model_factory = model_manufacturer.FindFactory(type_name);
    if (!model_factory) {
        std::cerr << "No model factory registered for type '" << type_name
                  << "'\n";
        return nullptr;
    }

    Rack *rack = dynamic_cast<Rack *>(model_factory->Produce(nullptr));
    if (!rack) {
        std::cerr << "Factory produced a non-Rack for type '" << type_name
                  << "'\n";
        return nullptr;
    }
    return rack;
}

// Deserialize JSON into a fresh rack constructed as a root (no parent).
Rack *DeserializeRack(const nlohmann::json &j) {
    if (!j.contains("type")) {
        std::cerr << "JSON missing 'type' field\n";
        return nullptr;
    }
    std::string type_name = j["type"].get<std::string>();

    // Construct the rack as a root via its model factory.
    /*
    auto &model_manufacturer = ModelManufacturer::singleton();
    ModelFactory *model_factory = model_manufacturer.FindFactory(type_name);
    if (!model_factory) {
        std::cerr << "No model factory registered for type '" << type_name
                  << "'\n";
        return nullptr;
    }

    Rack *rack = dynamic_cast<Rack *>(model_factory->Produce(nullptr));
    if (!rack) {
        std::cerr << "Factory produced a non-Rack for type '" << type_name
                  << "'\n";
        return nullptr;
    }
    */
    Rack *rack = NewRack(type_name);
    if (!rack) {
        std::cerr << "Failed to construct rack of type '" << type_name << "'\n";
        return nullptr;
    }

    // Run the archiver to populate state from JSON.
    auto &archiver_manufacturer = ArchiverManufacturer::singleton();
    ArchiverFactory *archiver_factory =
        archiver_manufacturer.FindFactory(type_name);
    if (!archiver_factory) {
        std::cerr << "No archiver factory registered for type '" << type_name
                  << "'\n";
        return rack;
    }

    Archiver *archiver = archiver_factory->Produce(*rack);
    Archive archive(const_cast<nlohmann::json &>(j));
    archiver->Load(archive);
    delete archiver;

    return rack;
}

bool RackDoc::Save(const std::filesystem::path &p) {
    try {
        auto j = SerializeRack(*rack_);
        std::ofstream out(p);
        out << j.dump(2);
        if (!out)
            return false;
        SetPath(p);
        MarkClean();
        return true;
    } catch (...) {
        return false;
    }
}

bool RackDoc::Load(const std::filesystem::path &p) {
    try {
        std::ifstream in(p);
        nlohmann::json j;
        in >> j;
        rack_ = DeserializeRack(j);
        SetPath(p);
        MarkClean();
        return true;
    } catch (...) {
        // audio left stopped on failure; caller decides
        return false;
    }
}

void RackDoc::NewDocument() {
    ClearPath();
    MarkClean();
    rack_ = NewRack("Rack");
}


} // namespace augr