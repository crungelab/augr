#pragma once

#include <augr/rack/archiver/subrack_archiver.h>

namespace augr {

class Voice;

class VoiceArchiver : public ArchiverT<Voice, SubrackArchiver> {
public:
    void Save(Archive &archive) const override;
    void Load(Archive &archive) override;
};

} // namespace augr