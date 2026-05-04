#pragma once

#include <map>
#include <memory>
#include <string>
#include <typeindex>
#include <vector>

namespace augr {

class Archiver;
class ArchiverFactory;
class Model;

// ArchiverManufacturer owns the registry of ArchiverFactory instances
// and produces archivers on demand. Lookup is by either std::type_index
// (when saving — we have a live Model) or by string type tag (when
// loading — we have a JSON tag).
class ArchiverManufacturer {
public:
    static ArchiverManufacturer &singleton() noexcept {
        static auto *self = new ArchiverManufacturer();
        return *self;
    }

    void AddFactory(ArchiverFactory &factory);

    [[nodiscard]] ArchiverFactory *FindFactory(const std::type_index &type) const;
    [[nodiscard]] ArchiverFactory *FindFactory(const std::string &type_name) const;

    // Convenience: find the factory and produce an archiver in one call.
    // Returns nullptr if no factory matches.
    [[nodiscard]] Archiver* MakeArchiver(Model &model) const;
    [[nodiscard]] Archiver* MakeArchiver(
        const std::string &type_name, Model &model) const;

    [[nodiscard]] const std::vector<ArchiverFactory *> &factories() const {
        return factories_;
    }

private:
    ArchiverManufacturer() = default;

    std::vector<ArchiverFactory *> factories_;
    std::map<std::type_index, ArchiverFactory *> by_type_;
    std::map<std::string, ArchiverFactory *> by_name_;
};

} // namespace augr