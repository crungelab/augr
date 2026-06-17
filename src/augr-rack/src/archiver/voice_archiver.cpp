#include <augr/rack/voice/voice.h>
#include <augr/rack/archiver/voice_archiver.h>

#include <augr/archiver_factory.h>
#include <nlohmann/json.hpp>

namespace augr {

void VoiceArchiver::Save(Archive &archive) const {
    SubrackArchiver::Save(archive);
}

void VoiceArchiver::Load(Archive &archive) {
    SubrackArchiver::Load(archive);
}

} // namespace augr

using namespace augr;

DEFINE_ARCHIVER_FACTORY(VoiceArchiver, Voice, "Voice")