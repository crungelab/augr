#include <augr/rack/voice/voice.h>
#include <augr/rack/voice/voicebank.h>

#include <augr/rack/archiver/voicebank_archiver.h>

#include <augr/core/archiver_factory.h>
#include <nlohmann/json.hpp>

namespace augr {

void VoicebankArchiver::Save(Archive &archive) const {
    GraphArchiver::Save(archive);

    const Voicebank &voicebank = subject();

    auto &j = archive.json();

    if (const auto master = voicebank.master()) {
        j["master_uuid"] = master->uuid_to_string();
    } else {
        j["master_uuid"] = std::string();
    }
}

void VoicebankArchiver::Load(Archive &archive) {
    GraphArchiver::Load(archive);

    Voicebank &voicebank = subject();
    const auto &j = archive.json();
    if (j.contains("master_uuid")) {
        archive.RegisterModuleResolver(
            j["master_uuid"].get<std::string>(), [j, &voicebank](Model *model) {
                if (auto *v = dynamic_cast<Voice *>(model)) {
                    voicebank.SetMaster(v);
                } else {
                    std::cerr << "Registered master uuid "
                              << j["master_uuid"].get<std::string>()
                              << " is not a Voice — ignoring\n";
                }
            });
    }
}

} // namespace augr

using namespace augr;

DEFINE_ARCHIVER_FACTORY(VoicebankArchiver, Voicebank, "Voicebank")