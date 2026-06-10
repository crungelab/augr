#pragma once

#include <map>
#include <memory>
#include <string>
#include <typeindex>
#include <vector>

#include "archiver_factory.h"
#include "manufacturer.h"

namespace augr {

class Archive;
class Archiver;
class Subject;

// ArchiverManufacturer owns the registry of ArchiverFactory instances
// and produces archivers on demand. Lookup is by either std::type_index
// (when saving — we have a live Subject) or by string type tag (when
// loading — we have a JSON tag).
class ArchiverManufacturer : public Manufacturer<ArchiverFactory> {
public:
    static ArchiverManufacturer &singleton() noexcept {
        static auto *self = new ArchiverManufacturer();
        return *self;
    }

    void Serialize(Archive &archive, Subject &subject) const;
    void Deserialize(Archive &archive, Subject &subject) const;
};

#define REGISTER_ARCHIVER_FACTORY(T)                                           \
    extern ArchiverFactory *Get##T##Factory();                                 \
    ArchiverManufacturer::singleton().AddFactory(*Get##T##Factory());

} // namespace augr