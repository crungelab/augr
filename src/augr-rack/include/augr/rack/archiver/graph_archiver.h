#pragma once

#include <augr/rack/archiver/module_archiver.h>

namespace augr {

class Graph;

class GraphArchiver : public ArchiverT<Graph, ModuleArchiver> {
public:
    void Save(Archive &archive) const override;
    void SaveChildren(Archive &archive) const;
    void SaveWires(Archive &archive) const;
    void Load(Archive &archive) override;
};

} // namespace augr