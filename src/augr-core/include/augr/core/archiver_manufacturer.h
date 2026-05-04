#pragma once

#include <map>
#include <string>
#include <typeindex>
#include <vector>

namespace augr {

class Archiver;

// ArchiverManufacturer owns the registry of Archiver instances and
// provides lookup by both std::type_index (for save, when we have a
// live Model and need its archiver) and by string type-name (for load,
// when we have a JSON type tag and need the matching archiver).
//
// Registration is explicit, not auto-registering — RegisterAllArchivers
// in a central .cpp file calls AddArchiver for each, ensuring linker
// dead-stripping doesn't drop archiver translation units.
class ArchiverManufacturer {
public:
    static ArchiverManufacturer &singleton() noexcept {
        static auto *self = new ArchiverManufacturer();
        return *self;
    }

    // Register an archiver. The archiver is owned by whoever defined it
    // (typically a static instance via DEFINE_ARCHIVER); the manufacturer
    // holds non-owning pointers.
    //
    // The type_name string is the JSON type tag this archiver handles
    // (e.g., "Operator"). It must match the type tag used by the
    // corresponding ModelFactory.
    void AddArchiver(Archiver &archiver, const std::string &type_name);

    // Find an archiver for a live Model's dynamic type. Used during save:
    //   manufacturer.Find(typeid(*module))->Save(*module, archive);
    [[nodiscard]] Archiver *Find(const std::type_index &type) const;

    // Find an archiver by JSON type tag. Used during load:
    //   manufacturer.Find(j["type"])->Load(*model, archive);
    [[nodiscard]] Archiver *Find(const std::string &type_name) const;

    // Iteration for diagnostics or tooling. Not expected on the hot path.
    [[nodiscard]] const std::vector<Archiver *> &archivers() const {
        return archivers_;
    }

private:
    ArchiverManufacturer() = default;

    std::vector<Archiver *> archivers_;
    std::map<std::type_index, Archiver *> by_type_;
    std::map<std::string, Archiver *> by_name_;
};

} // namespace augr