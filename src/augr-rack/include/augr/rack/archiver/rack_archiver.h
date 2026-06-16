#pragma once

#include <augr/rack/archiver/subrack_archiver.h>

namespace augr {

class Rack;

class RackArchiver : public ArchiverT<Rack, SubrackArchiver> {
public:
    void Save(Archive &archive) const override;
    void Load(Archive &archive) override;
};

} // namespace augr