// ModuleArchiver.cpp
#include <augr/rack/archiver/module_archiver.h>

#include <augr/core/archiver_factory.h>
#include <nlohmann/json.hpp>

namespace augr::rack {

void ModuleArchiver::Save(Archive &archive) const {
    auto &j = archive.json();
    const Module &op = model();
}

void ModuleArchiver::Load(Archive &archive) {
    const auto &j = archive.json();
    Module &op = model();
}

DEFINE_ARCHIVER_FACTORY(ModuleArchiver, Module, "Module")

} // namespace augr::rack