#include <uuid.h> // stduuid header

#include <augr/core/model.h>

namespace augr {

int Model::next_id_ = 0;

// File-scope generator. Threading: only the audio thread and the UI
// thread should ever construct subracks, and never simultaneously, so
// a single non-thread-safe generator is fine. If that assumption ever
// changes, wrap in a mutex.
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

std::string GenerateUuid() {
    uuids::uuid_random_generator gen{uuid_generator()};
    return uuids::to_string(gen());
}
} // namespace

const std::string &Model::uuid() const {
    if (uuid_.empty()) {
        uuid_ = GenerateUuid();
    }
    return uuid_;
}

void Model::RegenerateUuid() { uuid_ = GenerateUuid(); }

} // namespace augr