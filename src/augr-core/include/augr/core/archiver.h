#pragma once

#include <nlohmann/json.hpp>

#include <typeindex>
#include <utility>
#include <vector>

namespace augr {

class Model;

// Archive — same as before. Carries context that travels with a save
// or load operation; archivers reach through it for the underlying JSON
// plus version and module-resolution context.
class Archive {
public:
    explicit Archive(nlohmann::json &json, int version = 1)
        : json_(json), version_(version) {}

    [[nodiscard]] nlohmann::json &json() { return json_; }
    [[nodiscard]] const nlohmann::json &json() const { return json_; }

    [[nodiscard]] int version() const { return version_; }

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

// Archiver is instantiated per Model. Each Model has its own archiver,
// produced by an ArchiverFactory and bound to that Model at construction.
// Per-instance state (caches, intermediate values, references back to
// the Model) lives here.
class Archiver {
public:
    virtual ~Archiver() = default;

    virtual void Save(Archive &archive) const = 0;
    virtual void Load(Archive &archive) = 0;

    // Accessors
    virtual Model *model_ptr() = 0;
};

// ArchiverT — typed convenience layer. Subclasses of ArchiverT<T>
// see their Model as T& without writing static_casts.
template <typename T> class ArchiverT : public Archiver {
public:
    ArchiverT(T &model) : model_(&model) {}
    Model *model_ptr() override { return model_; }
    T &model() { return *model_; }
    const T &model() const { return *model_; }
    // Data members
    T *model_;
};

} // namespace augr