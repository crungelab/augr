#pragma once

#include <nlohmann/json.hpp>

#include <typeindex>
#include <utility>
#include <vector>

namespace augr {

class Model;

// Archive carries context that travels with a save or load operation.
// Most access goes through json(); the other accessors provide context
// that doesn't fit naturally inside the JSON itself (format version,
// module resolution table for connections).
class Archive {
public:
    explicit Archive(nlohmann::json &json, int version = 1)
        : json_(json), version_(version) {}

    // Direct access to the underlying JSON. This is the primary API.
    [[nodiscard]] nlohmann::json &json() { return json_; }
    [[nodiscard]] const nlohmann::json &json() const { return json_; }

    // Format version of the file being loaded (or written).
    // Archivers can branch on this to handle migrations.
    [[nodiscard]] int version() const { return version_; }

    // Resolve a module reference by its index in the file.
    // Populated by Rack load before per-connection archivers run.
    // Returns nullptr if the index is out of range — connection
    // archivers can treat null as "drop this connection," which is
    // also how the clipboard subset case handles connections to
    // modules outside the copied set.
    [[nodiscard]] Model *ResolveModule(int index) const {
        if (index < 0 || index >= static_cast<int>(resolved_modules_.size())) {
            return nullptr;
        }
        return resolved_modules_[index];
    }

    void SetResolvedModules(std::vector<Model *> modules) {
        resolved_modules_ = std::move(modules);
    }

private:
    nlohmann::json &json_;
    int version_;
    std::vector<Model *> resolved_modules_;
};

// Archiver is the base class for per-Model-type serialization logic.
// Each Model subclass T has a corresponding ArchiverT<T> subclass that
// implements SaveT / LoadT. Archivers are owned by ArchiverManufacturer
// and looked up by Model type at runtime.
class Archiver {
public:
    virtual ~Archiver() = default;

    virtual void Save(const Model &model, Archive &archive) const = 0;
    virtual void Load(Model &model, Archive &archive) const = 0;

    // The Model type this archiver handles. Used as the key in
    // ArchiverManufacturer's by-type map for save lookups.
    [[nodiscard]] virtual std::type_index ModelType() const = 0;
};

// ArchiverT downcasts Model& to T& and forwards to type-safe SaveT/LoadT
// methods that subclasses implement. Subclasses don't need to write
// static_casts in every method — they receive T& directly.
template <typename T> class ArchiverT : public Archiver {
public:
    void Save(const Model &model, Archive &archive) const override {
        SaveT(static_cast<const T &>(model), archive);
    }

    void Load(Model &model, Archive &archive) const override {
        LoadT(static_cast<T &>(model), archive);
    }

    [[nodiscard]] std::type_index ModelType() const override {
        return typeid(T);
    }

protected:
    virtual void SaveT(const T &model, Archive &archive) const = 0;
    virtual void LoadT(T &model, Archive &archive) const = 0;
};

} // namespace augr