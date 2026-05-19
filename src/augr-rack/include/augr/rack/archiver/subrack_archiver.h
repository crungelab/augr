#pragma once

#include <augr/rack/archiver/graph_archiver.h>

namespace augr {

class Subrack;

class SubrackArchiver : public ArchiverT<Subrack, GraphArchiver> {
public:
    void Save(Archive &archive) const override;
    void Load(Archive &archive) override;
};

} // namespace augr