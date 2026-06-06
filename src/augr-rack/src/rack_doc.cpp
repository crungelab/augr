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

constexpr const char *kDocumentFormatTag = "augr.document";
constexpr int kDocumentFormatVersion = 1;

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

std::unique_ptr<Rack> NewRack(const std::string &type_name,
                              CreateMode mode = CreateMode::Fresh) {
    auto &mm = ModelManufacturer::singleton();
    ModelFactory *mf = mm.FindFactory(type_name);
    if (!mf) {
        std::cerr << "No model factory registered for type '" << type_name
                  << "'\n";
        return nullptr;
    }
    Rack *raw = dynamic_cast<Rack *>(mf->Produce(nullptr, mode));
    if (!raw) {
        std::cerr << "Factory produced a non-Rack for type '" << type_name
                  << "'\n";
        return nullptr;
    }
    return std::unique_ptr<Rack>(raw);
}

std::unique_ptr<Rack> RackFromJson(const nlohmann::json &j) {
    if (!j.contains("type")) {
        std::cerr << "Rack JSON missing 'type' field\n";
        return nullptr;
    }
    std::string type_name = j["type"].get<std::string>();

    auto rack = NewRack(type_name, CreateMode::Loaded);
    if (!rack)
        return nullptr;

    auto &am = ArchiverManufacturer::singleton();
    ArchiverFactory *af = am.FindFactory(type_name);
    if (!af) {
        std::cerr << "No archiver factory registered for type '" << type_name
                  << "'\n";
        return nullptr;
    }
    std::unique_ptr<Archiver> archiver(af->Produce(*rack));

    nlohmann::json local = j;
    Archive archive(local);
    archiver->Load(archive);

    return rack;
}

// Validates the envelope and returns a pointer to the rack subtree within it.
// Accepts legacy bare-rack files (no "format" key) for backward compatibility.
bool ParseEnvelope(const nlohmann::json &doc,
                   const nlohmann::json *&out_rack_json) {
    if (!doc.contains("format")) {
        out_rack_json = &doc;
        return true;
    }
    if (!doc["format"].is_string() ||
        doc["format"].get<std::string>() != kDocumentFormatTag) {
        std::cerr << "Unknown document format tag\n";
        return false;
    }
    int version = doc.value("version", 0);
    if (version <= 0 || version > kDocumentFormatVersion) {
        std::cerr << "Unsupported document version: " << version << "\n";
        return false;
    }
    if (!doc.contains("rack") || !doc["rack"].is_object()) {
        std::cerr << "Document missing 'rack' subobject\n";
        return false;
    }
    out_rack_json = &doc["rack"];
    return true;
}

} // namespace

RackDoc::~RackDoc() { Stop(); }

bool RackDoc::Save(const std::filesystem::path &p) {
    try {
        nlohmann::json rack_json;
        if (!RackToJson(rack(), rack_json))
            return false;

        data_ = nlohmann::json::object();
        data_["format"] = kDocumentFormatTag;
        data_["version"] = kDocumentFormatVersion;
        data_["rack"] = std::move(rack_json);

        // Let subscribers (e.g. frames) push live view state into views_.
        on_save();

        nlohmann::json views_json = nlohmann::json::object();
        for (const auto &[uuid, view_json] : views_) {
            views_json[uuid] = view_json;
        }
        data_["views"] = std::move(views_json);

        std::ofstream out(p);
        out << data_.dump(2);
        if (!out) {
            std::cerr << "Save: write failed for " << p << "\n";
            data_ = {};
            return false;
        }

        SetPath(p);
        MarkClean();
        data_ = {};
        return true;
    } catch (const std::exception &e) {
        std::cerr << "Save failed: " << e.what() << "\n";
        data_ = {};
        return false;
    }
}

// Stops the current rack, fires on_unload, installs the new rack, populates
// data_, fires on_load, then clears data_. All rack replacement goes through
// here so subscribers see a consistent sequence.
void RackDoc::ReplaceRack(std::unique_ptr<Rack> fresh,
                          nlohmann::json envelope) {
    Stop();
    on_unload();
    model_ = std::move(fresh);
    data_ = std::move(envelope);
    on_load();
    data_ = {};
}

bool RackDoc::Load(const std::filesystem::path &p, bool auto_start) {
    try {
        std::ifstream in(p);
        if (!in) {
            std::cerr << "Load: could not open " << p << "\n";
            return false;
        }

        nlohmann::json doc;
        in >> doc;

        const nlohmann::json *rack_json = nullptr;
        if (!ParseEnvelope(doc, rack_json))
            return false;

        auto fresh = RackFromJson(*rack_json);
        if (!fresh)
            return false;

        views_.clear();
        if (doc.contains("views") && doc["views"].is_object()) {
            for (auto &[uuid, view_json] : doc["views"].items()) {
                views_[uuid] = view_json;
            }
        }

        ReplaceRack(std::move(fresh), std::move(doc));

        SetPath(p);
        MarkClean();
        if (auto_start)
            Start();
        return true;
    } catch (const std::exception &e) {
        std::cerr << "Load failed: " << e.what() << "\n";
        return false;
    }
}

void RackDoc::NewDocument(bool auto_start) {
    auto fresh = NewRack("Rack");
    if (!fresh)
        return;

    views_.clear();
    ClearPath();
    MarkClean();

    // Synthesize a minimal envelope so on_load subscribers see a consistent
    // data_ shape (empty views, no rack content to read).
    nlohmann::json synthetic = nlohmann::json::object();
    synthetic["format"] = kDocumentFormatTag;
    synthetic["version"] = kDocumentFormatVersion;
    synthetic["views"] = nlohmann::json::object();

    ReplaceRack(std::move(fresh), std::move(synthetic));

    if (auto_start)
        Start();
}

bool RackDoc::Start() {
    if (!model_)
        return false;
    if (model_->IsRunning())
        return true;
    try {
        model_->Start();
        return true;
    } catch (const std::exception &e) {
        std::cerr << "RackDoc::Start failed: " << e.what() << "\n";
        return false;
    }
}

void RackDoc::Stop() {
    if (model_ && model_->IsRunning())
        model_->Stop();
}

bool RackDoc::IsRunning() const { return model_ && model_->IsRunning(); }

} // namespace augr