#include <augr/rack/voice/voice.h>
#include <augr/rack/archiver/voice_archiver.h>

#include <augr/core/archiver_factory.h>
#include <nlohmann/json.hpp>

namespace augr {

void VoiceArchiver::Save(Archive &archive) const {
    GraphArchiver::Save(archive);
}

void VoiceArchiver::Load(Archive &archive) {
    GraphArchiver::Load(archive);
}

} // namespace augr

using namespace augr;

DEFINE_ARCHIVER_FACTORY(VoiceArchiver, Voice, "Voice")