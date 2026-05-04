// ModuleArchiver.cpp
#include <augr/rack/archiver/rack_archiver.h>

#include <augr/core/archiver_factory.h>
#include <nlohmann/json.hpp>

namespace augr {

void RackArchiver::Save(Archive &archive) const {
    //auto &j = archive.json();
    nlohmann::json j_module;
    const Module &module = model();

    for (const auto &param : module.parameters()) {
        nlohmann::json j_param;
        j_param["label"] = param->label();
        j_param["value"] = param->GetNormalized();
        j_module["parameters"].push_back(j_param);
    }
}

void RackArchiver::Load(Archive &archive) {
    const auto &j = archive.json();
    Module &module = model();
}

} // namespace augr

using namespace augr;

DEFINE_ARCHIVER_FACTORY(RackArchiver, Module, "Module")