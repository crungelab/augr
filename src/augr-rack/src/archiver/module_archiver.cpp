//#include <augr/core/model_registry.h>

#include <augr/rack/archiver/module_archiver.h>

#include <augr/core/archiver_factory.h>
#include <nlohmann/json.hpp>

namespace augr {

void ModuleArchiver::Save(Archive &archive) const {
    Archiver::Save(archive);

    auto &j = archive.json();
    const Module &module = subject();

    if (module.is_replicated()) {
        std::cerr << "Warning: Saving a replicated module — this is not a "
                     "supported use case and may produce unexpected results.\n";
    }

    // Persist the module's stable identity. Lazy-minted on first call,
    // so accessing uuid() here ensures every saved module has one.
    j["uuid"] = module.uuid_to_string();

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

    // Restore the module's stable identity. If missing (legacy files),
    // the uuid will be lazily minted on first access — which is fine
    // for round-trip but means the loaded module gets a new identity
    // rather than preserving its original.
    if (j.contains("uuid") && j["uuid"].is_string()) {
        module.set_uuid(j["uuid"].get<std::string>());
    }

    //ModelRegistry::singleton().RegisterWithUuid(&module, module.uuid());
    archive.RegisterModule(module.uuid(), &module);

    // Type tag is read by the caller (it's needed before we can construct
    // this Module), so we don't read it here.

    if (j.contains("label")) {
        module.set_label(j["label"].get<std::string>());
    }

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