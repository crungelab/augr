#pragma once

#include <augr/rack/archiver/module_archiver.h>

namespace augr {

class Graph;

class GraphArchiver : public ArchiverT<Graph, ModuleArchiver> {
public:
    void Save(Archive &archive) const override;
    void SaveChildren(Archive &archive,
                      const std::vector<Model::Ptr> &modules) const;
    void SaveWires(Archive &archive,
                   const std::vector<Model::Ptr> &modules) const;
    void Load(Archive &archive) override;
    void LoadChildren(Archive &archive);
    void LoadWires(Archive &archive);
};

} // namespace augr