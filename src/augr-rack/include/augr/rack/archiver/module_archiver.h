#pragma once

#include <augr/core/archiver.h>
#include <augr/rack/module/module.h>

namespace augr {

class ModuleArchiver : public ArchiverT<Module> {
public:
    void Save(Archive &archive) const override;
    void Load(Archive &archive) override;
};

} // namespace augr