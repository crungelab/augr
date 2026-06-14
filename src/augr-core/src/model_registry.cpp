#include <random>

#include <uuid.h>

#include <augr/core/model.h>
#include <augr/core/model_registry.h>

namespace augr {

namespace {
std::mt19937 &uuid_generator() {
    static std::mt19937 gen([]() {
        std::random_device rd;
        std::array<int, std::mt19937::state_size> seed_data;
        std::generate(seed_data.begin(), seed_data.end(), std::ref(rd));
        std::seed_seq seq(seed_data.begin(), seed_data.end());
        return std::mt19937(seq);
    }());
    return gen;
}
} // namespace

ModelRegistry &ModelRegistry::singleton() {
    static ModelRegistry instance;
    return instance;
}

Uuid ModelRegistry::GenerateUuid() {
    uuids::uuid_random_generator gen{uuid_generator()};
    return gen();
}

Uuid ModelRegistry::Register(Model *model) {
    Uuid uuid = GenerateUuid();
    registry_[uuid] = model;
    return uuid;
}

void ModelRegistry::RegisterWithUuid(Model *model, const Uuid &uuid) {
    // Remove any stale entry pointing to this model under its old UUID.
    for (auto it = registry_.begin(); it != registry_.end(); ) {
        if (it->second == model) {
            it = registry_.erase(it);
        } else {
            ++it;
        }
    }
    registry_[uuid] = model;
}

void ModelRegistry::Deregister(const Uuid &uuid) {
    registry_.erase(uuid);
}

Model *ModelRegistry::Find(const Uuid &uuid) const {
    auto it = registry_.find(uuid);
    return it != registry_.end() ? it->second : nullptr;
}

std::string ModelRegistry::ToString(const Uuid &uuid) {
    return uuids::to_string(uuid);
}

Uuid ModelRegistry::FromString(const std::string &str) {
    return uuids::uuid::from_string(str).value_or(uuids::uuid{});
}

} // namespace augr