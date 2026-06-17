#pragma once

#include <map>
#include <string>

namespace augr {

struct Meta : std::map<std::string, std::string> {
    void declare(const char *key, const char *value) { (*this)[key] = value; }

    const char *get(const char *key, const char *def = nullptr) const {
        auto it = find(key);
        return it == end() ? def : it->second.c_str();
    }

    bool has(const char *key) const { return find(key) != end(); }
};

} // namespace augr