#include <augr/rack/archiver/module_archiver.h>

#include <augr/core/archiver_factory.h>
#include <nlohmann/json.hpp>

namespace augr {

void ModuleArchiver::Save(Archive &archive) const {
    auto &j = archive.json();
    const Module &module = subject();

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
    Module &module = subject();

    // Type tag is read by the caller (it's needed before we can construct
    // this Module), so we don't read it here.

    if (j.contains("label")) {
        module.set_label(j["label"].get<std::string>());
    }
    /*
    if (j.contains("position")) {
        const auto &j_pos = j["position"];
        if (j_pos.is_array() && j_pos.size() == 2) {
            module.SetPosition(j_pos[0].get<float>(), j_pos[1].get<float>());
        }
    }
    */

    if (j.contains("parameters")) {
        const auto &j_params = j["parameters"];
        for (auto &param : subject().parameters()) {
            const std::string &name = param->label();
            if (j_params.contains(name)) {
                param->SetNormalized(j_params[name].get<float>());
            }
        }
    }
}

} // namespace augr

using namespace augr;

DEFINE_ARCHIVER_FACTORY(ModuleArchiver, Module, "Module")