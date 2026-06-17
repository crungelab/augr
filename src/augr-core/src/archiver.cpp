#include <augr/archiver.h>
#include <augr/archiver_factory.h>

namespace augr {

void Archiver::Save(Archive &archive) const {
    auto &j = archive.json();

    j["type"] = factory_->name();
}

void Archiver::Load(Archive &archive) {
    // Base implementation does nothing. Subclasses override this.
}

} // namespace augr