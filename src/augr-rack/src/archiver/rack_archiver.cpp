#include <augr/rack/rack.h>
#include <augr/rack/archiver/rack_archiver.h>

#include <augr/archiver_factory.h>
#include <nlohmann/json.hpp>

namespace augr {

void RackArchiver::Save(Archive &archive) const {
    SubrackArchiver::Save(archive);
}

void RackArchiver::Load(Archive &archive) {
    SubrackArchiver::Load(archive);
}

} // namespace augr

using namespace augr;

DEFINE_ARCHIVER_FACTORY(RackArchiver, Rack, "Rack")