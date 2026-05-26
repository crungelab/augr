#pragma once

#include <augr/rack/archiver/graph_archiver.h>

namespace augr {

class Voice;

class VoiceArchiver : public ArchiverT<Voice, GraphArchiver> {
public:
    void Save(Archive &archive) const override;
    void Load(Archive &archive) override;
};

} // namespace augr