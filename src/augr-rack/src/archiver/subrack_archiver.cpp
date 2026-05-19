#include <augr/rack/subrack.h>
#include <augr/rack/archiver/subrack_archiver.h>

#include <augr/core/archiver_factory.h>
#include <nlohmann/json.hpp>

namespace augr {

void SubrackArchiver::Save(Archive &archive) const {
    GraphArchiver::Save(archive);
}

void SubrackArchiver::Load(Archive &archive) {
    GraphArchiver::Load(archive);
}

} // namespace augr

using namespace augr;

DEFINE_ARCHIVER_FACTORY(SubrackArchiver, Subrack, "Subrack")