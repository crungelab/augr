#include <random>

#include <uuid.h>

#include <augr/core/model.h>

namespace augr {

int Model::next_id_ = 0;

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

uuids::uuid GenerateUuid() {
    uuids::uuid_random_generator gen{uuid_generator()};
    return gen();
}
} // namespace

Model::~Model() {}

void Model::OnCreateFresh() {
    uuid_ = GenerateUuid();
}

void Model::OnCreateLoaded() {
    // uuid_ already set by archiver via set_uuid() before this is called.
    // Nothing to do here.
}

void Model::OnCreateCopied() {
    uuid_ = GenerateUuid();
}

void Model::OnCreateReplicated() {
    // uuid copied
}

Model *Model::FindByUuid(const uuids::uuid &uuid) {
    if (uuid_ == uuid)
        return this;
    for (const auto &child : children_) {
        if (Model *found = child->FindByUuid(uuid))
            return found;
    }
    return nullptr;
}

} // namespace augr