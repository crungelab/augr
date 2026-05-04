// OperatorArchiver.h
#pragma once

#include <augr/core/archiver.h>
#include <augr/rack/module/module.h>

namespace augr::rack {

class ModuleArchiver : public ArchiverT<Module> {
public:
    ModuleArchiver(Module &model) : ArchiverT<Module>(model) {}
    void Save(Archive &archive) const override;
    void Load(Archive &archive) override;
};

} // namespace augr::rack