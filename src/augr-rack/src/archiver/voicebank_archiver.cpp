#include <augr/rack/voice/voicebank.h>
#include <augr/rack/archiver/voicebank_archiver.h>

#include <augr/core/archiver_factory.h>
#include <nlohmann/json.hpp>

namespace augr {

void VoicebankArchiver::Save(Archive &archive) const {
    GraphArchiver::Save(archive);
}

void VoicebankArchiver::Load(Archive &archive) {
    GraphArchiver::Load(archive);
}

} // namespace augr

using namespace augr;

DEFINE_ARCHIVER_FACTORY(VoicebankArchiver, Voicebank, "Voicebank")