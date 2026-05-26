#pragma once

#include <augr/rack/archiver/graph_archiver.h>

namespace augr {

class Voicebank;

class VoicebankArchiver : public ArchiverT<Voicebank, GraphArchiver> {
public:
    void Save(Archive &archive) const override;
    void Load(Archive &archive) override;
};

} // namespace augr