#pragma once

#include <map>
#include <string>
#include <typeindex>
#include <vector>

#include "model_factory.h"

namespace augr {

template <typename TFactory> class Manufacturer {
public:
    void AddFactory(TFactory &factory) {
        factories_.push_back(&factory);
        factory_type_map_[factory.GetKey()] = &factory;
        factory_name_map_[factory.name()] = &factory;
    }

    TFactory *FindFactory(const std::string &type_name) const {
        auto it = factory_name_map_.find(type_name);
        return it == factory_name_map_.end() ? nullptr : it->second;
    }

    TFactory *GetFactory(const std::type_index &key) const { return FindFactory(key); }

    TFactory *FindFactory(const std::type_index &t) const {
        // exact match
        if (auto it = factory_type_map_.find(t); it != factory_type_map_.end())
            return it->second;

        // search bases (depth-first)
        for (const auto &b : ::reflect::Registry::singleton().bases_of(t)) {
            if (auto *f = FindFactory(b))
                return f;
        }
        return nullptr;
    }

    // Data members
    std::vector<TFactory *> factories_;
    std::map<std::type_index, TFactory *> factory_type_map_;
    std::map<std::string, TFactory *> factory_name_map_;
};

} // namespace augr