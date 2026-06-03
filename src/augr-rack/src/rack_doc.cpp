// rack_doc.cpp
#include <augr/rack/rack_doc.h>

#include <algorithm>
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
std::unique_ptr<Rack> NewRack(const std::string &type_name, CreateMode mode = CreateMode::Fresh) {
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

// Pure: json -> rack (takes the bare rack subtree, not the envelope).
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

// Validates the envelope and extracts the rack subtree. Returns false on
// structural errors. Accepts legacy bare-rack files (no envelope) for
// backward compatibility — detected by absence of the "format" tag.
//
// Hook subtrees aren't extracted here; load hooks read directly from the
// full document JSON by their registered key.
bool ParseEnvelope(const nlohmann::json &doc,
                   const nlohmann::json *&out_rack_json) {
    // Legacy path: file is just a bare rack (no envelope). Treat the
    // whole document as the rack subtree.
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

        nlohmann::json doc = nlohmann::json::object();
        doc["format"] = kDocumentFormatTag;
        doc["version"] = kDocumentFormatVersion;
        doc["rack"] = std::move(rack_json);

        // Fire save hooks before serializing views_, so frames can
        // push their live state into views_ first.
        for (const auto &hook : save_hooks_) {
            try {
                hook.fn(doc);
            } catch (const std::exception &e) {
                std::cerr << "Save hook threw: " << e.what() << "\n";
            }
        }

        nlohmann::json views_json = nlohmann::json::object();
        for (const auto &[uuid, view_json] : views_) {
            views_json[uuid] = view_json;
        }
        doc["views"] = std::move(views_json);

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
        if (!ParseEnvelope(doc, rack_json)) {
            return false;
        }

        auto fresh = RackFromJson(*rack_json);
        if (!fresh)
            return false;

        views_.clear();
        if (doc.contains("views") && doc["views"].is_object()) {
            for (auto &[uuid, view_json] : doc["views"].items()) {
                views_[uuid] = view_json;
            }
        }

        Stop();
        model_ = std::move(fresh);

        SetPath(p);
        MarkClean();

        // Fire load hooks after the rack and views_ are in place.
        // Hooks can pull from views_ via the doc reference.
        for (const auto &hook : load_hooks_) {
            try {
                hook.fn(doc);
            } catch (const std::exception &e) {
                std::cerr << "Load hook threw: " << e.what() << "\n";
            }
        }

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
    if (!fresh) { /* ... */
    }

    Stop();
    model_ = std::move(fresh);
    views_.clear();
    ClearPath();
    MarkClean();

    // Notify load hooks with an empty envelope so subsystems reset to
    // defaults. Synthesize a minimal doc-shaped json since we don't
    // have a real envelope here.
    nlohmann::json synthetic = nlohmann::json::object();
    synthetic["format"] = kDocumentFormatTag;
    synthetic["version"] = kDocumentFormatVersion;
    synthetic["views"] = nlohmann::json::object();

    for (const auto &hook : load_hooks_) {
        try {
            hook.fn(synthetic);
        } catch (const std::exception &e) {
            std::cerr << "Load hook threw on NewDocument: " << e.what() << "\n";
        }
    }

    if (auto_start) {
        Start();
    }
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
    if (model_ && model_->IsRunning()) {
        model_->Stop();
    }
}

bool RackDoc::IsRunning() const { return model_ && model_->IsRunning(); }

} // namespace augr