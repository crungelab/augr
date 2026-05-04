// ModuleArchiver.cpp

#include <augr/rack/archiver/module_archiver.h>

#include <augr/core/archiver_factory.h>
#include <nlohmann/json.hpp>

namespace augr {

void ModuleArchiver::Save(Archive &archive) const {
    auto &j = archive.json();
    const Module &module = model();

    j["type"] = factory_->type_name();

    if (!module.parameters().empty()) {
        auto &j_params = j["parameters"];
        for (const auto &param : module.parameters()) {
            j_params[param->label()] = param->GetNormalized();
        }
    }
}

void ModuleArchiver::Load(Archive &archive) {
    const auto &j = archive.json();
    Module &module = model();
}

} // namespace augr

using namespace augr;

DEFINE_ARCHIVER_FACTORY(ModuleArchiver, Module, "Module")