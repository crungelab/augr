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

// Document envelope constants. Bump kDocumentVersion on breaking shape
// changes to the envelope itself (not to the rack subtree, which has its
// own versioning concerns via GraphArchiver).
constexpr const char *kDocumentFormatTag = "augr.document";
constexpr int kDocumentFormatVersion = 1;

// Pure: rack -> json (bare rack json, no envelope).
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

// Pure: json -> rack (takes the bare rack subtree, not the envelope).
std::unique_ptr<Rack> RackFromJson(const nlohmann::json &j) {
    if (!j.contains("type")) {
        std::cerr << "Rack JSON missing 'type' field\n";
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
        return nullptr;
    }
    std::unique_ptr<Archiver> archiver(af->Produce(*rack));

    nlohmann::json local = j;
    Archive archive(local);
    archiver->Load(archive);

    return rack;
}

// Wraps a rack-json subtree in the document envelope.
nlohmann::json BuildEnvelope(nlohmann::json rack_json,
                             const nlohmann::json &metadata) {
    nlohmann::json doc = nlohmann::json::object();
    doc["format"] = kDocumentFormatTag;
    doc["version"] = kDocumentFormatVersion;
    doc["rack"] = std::move(rack_json);
    if (!metadata.empty()) {
        // Spread metadata as top-level siblings of "rack". This keeps
        // additions like "editor", "midi_learn", etc. at the top of the
        // file rather than nested under a generic "metadata" key.
        for (auto it = metadata.begin(); it != metadata.end(); ++it) {
            // Don't let metadata clobber reserved envelope keys.
            if (it.key() == "format" || it.key() == "version" ||
                it.key() == "rack") {
                std::cerr << "Skipping reserved metadata key '" << it.key()
                          << "'\n";
                continue;
            }
            doc[it.key()] = it.value();
        }
    }
    return doc;
}

// Validates the envelope and extracts (rack_json, metadata). Returns false
// on structural errors. Accepts legacy bare-rack files (no envelope) for
// backward compatibility — detected by absence of the "format" tag.
bool ParseEnvelope(const nlohmann::json &doc,
                   const nlohmann::json *&out_rack_json,
                   nlohmann::json &out_metadata) {
    out_metadata = nlohmann::json::object();

    // Legacy path: file is just a bare rack (no envelope). Treat the
    // whole document as the rack subtree and return empty metadata.
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

    // Everything except the reserved keys goes into metadata.
    for (auto it = doc.begin(); it != doc.end(); ++it) {
        if (it.key() == "format" || it.key() == "version" ||
            it.key() == "rack") {
            continue;
        }
        out_metadata[it.key()] = it.value();
    }
    return true;
}

} // namespace

RackDoc::~RackDoc() {
    Stop();
}

nlohmann::json &RackDoc::MetadataSection(const std::string &key) {
    auto it = metadata_.find(key);
    if (it == metadata_.end() || !it->is_object()) {
        metadata_[key] = nlohmann::json::object();
    }
    return metadata_[key];
}

bool RackDoc::Save(const std::filesystem::path &p) {
    try {
        nlohmann::json rack_json;
        if (!RackToJson(rack(), rack_json))
            return false;

        nlohmann::json doc = BuildEnvelope(std::move(rack_json), metadata_);

        std::ofstream out(p);
        out << doc.dump(2);
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
        nlohmann::json metadata;
        if (!ParseEnvelope(doc, rack_json, metadata)) {
            return false;
        }

        auto fresh = RackFromJson(*rack_json);
        if (!fresh) return false;

        Stop();
        model_ = std::move(fresh);
        metadata_ = std::move(metadata);

        SetPath(p);
        MarkClean();

        if (auto_start) {
            Start();
        }
        return true;
    } catch (const std::exception &e) {
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
    metadata_ = nlohmann::json::object();
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
    } catch (const std::exception &e) {
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