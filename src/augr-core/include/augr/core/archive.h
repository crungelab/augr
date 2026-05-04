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

} // namespace augr