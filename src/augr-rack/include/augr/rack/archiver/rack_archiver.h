#pragma once

#include <augr/core/archiver.h>
#include <augr/rack/module/module.h>

namespace augr {

class RackArchiver : public ArchiverT<Module> {
public:
    RackArchiver(Module &model) : ArchiverT<Module>(model) {}
    void Save(Archive &archive) const override;
    void Load(Archive &archive) override;
};

} // namespace augr