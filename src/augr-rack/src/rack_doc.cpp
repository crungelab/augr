// rack_doc.cpp
#include <augr/rack/rack_doc.h>

#include <fstream>
#include <iostream>

#include <nlohmann/json.hpp>

#include <augr/core/archive.h>
#include <augr/core/archiver.h>
#include <augr/core/archiver_factory.h>
#include <augr/core/archiver_manufacturer.h>
#include <augr/core/model_manufacturer.h>
#include <augr/rack/rack.h>

namespace augr {

namespace {

// Pure: rack -> json. No I/O, no printing.
bool RackToJson(Rack &rack, nlohmann::json &out_json) {
    auto &manufacturer = ArchiverManufacturer::singleton();
    auto *factory = manufacturer.FindFactory(std::type_index(typeid(rack)));
    if (!factory) {
        std::cerr << "No archiver factory registered for type "
                  << typeid(rack).name() << "\n";
        return false;
    }
    std::unique_ptr<Archiver> archiver(factory->Produce(rack));
    if (!archiver) {
        std::cerr << "Failed to construct archiver\n";
        return false;
    }
    Archive archive(out_json);
    archiver->Save(archive);
    return true;
}

// Construct a fresh root Rack of the named type.
std::unique_ptr<Rack> NewRack(const std::string &type_name) {
    auto &mm = ModelManufacturer::singleton();
    ModelFactory *mf = mm.FindFactory(type_name);
    if (!mf) {
        std::cerr << "No model factory registered for type '" << type_name
                  << "'\n";
        return nullptr;
    }
    Rack *raw = dynamic_cast<Rack *>(mf->Produce(nullptr));
    if (!raw) {
        std::cerr << "Factory produced a non-Rack for type '" << type_name
                  << "'\n";
        return nullptr;
    }
    return std::unique_ptr<Rack>(raw);
}

// Pure: json -> rack. Returns nullptr on failure.
std::unique_ptr<Rack> RackFromJson(const nlohmann::json &j) {
    if (!j.contains("type")) {
        std::cerr << "JSON missing 'type' field\n";
        return nullptr;
    }
    std::string type_name = j["type"].get<std::string>();

    auto rack = NewRack(type_name);
    if (!rack)
        return nullptr;

    auto &am = ArchiverManufacturer::singleton();
    ArchiverFactory *af = am.FindFactory(type_name);
    if (!af) {
        std::cerr << "No archiver factory registered for type '" << type_name
                  << "'\n";
        return nullptr; // Fail loudly rather than returning a half-built rack.
    }
    std::unique_ptr<Archiver> archiver(af->Produce(*rack));

    // Archive ideally takes a const json& for loads. If your Archive type
    // requires non-const, keep a local copy here rather than const_cast'ing.
    nlohmann::json local = j;
    Archive archive(local);
    archiver->Load(archive);

    return rack;
}

} // namespace

RackDoc::~RackDoc() {
    Stop();   // belt-and-suspenders; Rack's dtor also stops, but be explicit.
}

bool RackDoc::Save(const std::filesystem::path &p) {
    try {
        nlohmann::json j;
        if (!RackToJson(rack(), j))
            return false;
        std::ofstream out(p);
        out << j.dump(2);
        if (!out) {
            std::cerr << "Save: write failed for " << p << "\n";
            return false;
        }
        SetPath(p);
        MarkClean();
        return true;
    } catch (const std::exception &e) {
        std::cerr << "Save failed: " << e.what() << "\n";
        return false;
    }
}

bool RackDoc::Load(const std::filesystem::path& p, bool auto_start) {
    try {
        std::ifstream in(p);
        if (!in) {
            std::cerr << "Load: could not open " << p << "\n";
            return false;
        }
        nlohmann::json j;
        in >> j;

        auto fresh = RackFromJson(j);
        if (!fresh) return false;

        // Stop old, swap, optionally start new.
        Stop();
        model_ = std::move(fresh);

        SetPath(p);
        MarkClean();

        if (auto_start) {
            Start();   // failures are logged but don't fail the load
        }
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Load failed: " << e.what() << "\n";
        return false;
    }
}

void RackDoc::NewDocument(bool auto_start) {
    auto fresh = NewRack("Rack");
    if (!fresh) {
        std::cerr << "NewDocument: failed to construct default Rack\n";
        return;
    }
    fresh->CreateDefaultDevices();

    Stop();
    model_ = std::move(fresh);
    ClearPath();
    MarkClean();

    if (auto_start) {
        Start();
    }
}

bool RackDoc::Start() {
    if (!model_) return false;
    if (model_->IsRunning()) return true;
    try {
        model_->Start();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "RackDoc::Start failed: " << e.what() << "\n";
        return false;
    }
}

void RackDoc::Stop() {
    if (model_ && model_->IsRunning()) {
        model_->Stop();
    }
}

bool RackDoc::IsRunning() const {
    return model_ && model_->IsRunning();
}

} // namespace augr