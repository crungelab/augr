#pragma once

#include <map>
#include <memory>
#include <string>
#include <typeindex>
#include <vector>

#include "archiver_factory.h"

namespace augr {

class Archive;
class Archiver;
class Subject;

// ArchiverManufacturer owns the registry of ArchiverFactory instances
// and produces archivers on demand. Lookup is by either std::type_index
// (when saving — we have a live Subject) or by string type tag (when
// loading — we have a JSON tag).
class ArchiverManufacturer {
public:
    static ArchiverManufacturer &singleton() noexcept {
        static auto *self = new ArchiverManufacturer();
        return *self;
    }

    void AddFactory(ArchiverFactory &factory);

    ArchiverFactory *FindFactory(const std::type_index &type) const;
    ArchiverFactory *FindFactory(const std::string &type_name) const;

    // Convenience: find the factory and produce an archiver in one call.
    // Returns nullptr if no factory matches.
    Archiver *MakeArchiver(Subject &subject) const;
    Archiver *MakeArchiver(const std::string &type_name, Subject &subject) const;

    void Serialize(Archive &archive, Subject &subject) const;
    void Deserialize(Archive &archive, Subject &subject) const;

    const std::vector<ArchiverFactory *> &factories() const {
        return factories_;
    }

private:
    ArchiverManufacturer() = default;

    std::vector<ArchiverFactory *> factories_;
    std::map<std::type_index, ArchiverFactory *> factory_type_map_;
    std::map<std::string, ArchiverFactory *> factory_name_map_;
};

#define REGISTER_ARCHIVER_FACTORY(T)                                           \
    extern ArchiverFactory *Get##T##Factory();                                 \
    ArchiverManufacturer::singleton().AddFactory(*Get##T##Factory());

} // namespace augr