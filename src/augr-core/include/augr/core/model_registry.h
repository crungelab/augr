#pragma once

#include <string>
#include <unordered_map>

#include <uuid.h>

namespace augr {

class Model;

using Uuid = uuids::uuid;

class ModelRegistry {
public:
    static ModelRegistry &singleton();

    // Generates a new UUID, registers the model, and returns the UUID.
    Uuid Register(Model *model);

    // Re-registers a model under a specific UUID (load path).
    // Deregisters any previous UUID the model held.
    void RegisterWithUuid(Model *model, const Uuid &uuid);

    void Deregister(const Uuid &uuid);

    Model *Find(const Uuid &uuid) const;

    static std::string ToString(const Uuid &uuid);
    static Uuid FromString(const std::string &str);

private:
    ModelRegistry() = default;

    Uuid GenerateUuid();

    std::unordered_map<Uuid, Model *> registry_;
};

} // namespace augr