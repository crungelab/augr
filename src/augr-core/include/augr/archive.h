#pragma once

#include <nlohmann/json.hpp>

#include <algorithm>
#include <functional>
#include <list>
#include <map>
#include <typeindex>
#include <utility>
#include <vector>

#include <augr/model.h>

namespace augr {

using ModuleResolver = std::function<void(Model *)>;

class Archive {
public:
    explicit Archive(nlohmann::json &root, int version = 1)
        : version_(version) {
        json_stack_.push_back(&root);
    }

    nlohmann::json &json() { return *json_stack_.back(); }
    const nlohmann::json &json() const { return *json_stack_.back(); }

    int version() const { return version_; }

    void PushJson(nlohmann::json &j) { json_stack_.push_back(&j); }
    void PopJson() { json_stack_.pop_back(); }

    void PushGraph(const std::vector<Model::Ptr> &modules) {
        // Store raw non-owning pointers for index resolution.
        std::vector<Model *> raw;
        raw.reserve(modules.size());
        for (const auto &m : modules)
            raw.push_back(m.get());
        graph_stack_.push_back(std::move(raw));
    }

    void PopGraph() { graph_stack_.pop_back(); }

    Model *ResolveModule(int index) const {
        if (graph_stack_.empty())
            return nullptr;
        const auto &current = graph_stack_.back();
        if (index < 0 || index >= static_cast<int>(current.size()))
            return nullptr;
        return current[index];
    }

    int IndexOf(const Model *model) const {
        if (graph_stack_.empty())
            return -1;
        const auto &current = graph_stack_.back();
        auto it = std::find(current.begin(), current.end(), model);
        return it == current.end() ? -1
                                   : static_cast<int>(it - current.begin());
    }

    void RegisterModuleResolver(const uuids::uuid &uuid,
                                ModuleResolver resolver) {
        if (const auto it = module_map_.find(uuid); it != module_map_.end()) {
            resolver(it->second);
            return;
        }
        module_resolver_map_[uuid].push_back(std::move(resolver));
    }

    void RegisterModuleResolver(const std::string &uuid_str,
                                ModuleResolver resolver) {
        RegisterModuleResolver(
            uuids::uuid::from_string(uuid_str).value_or(uuids::uuid{}),
            std::move(resolver));
    }

    void RegisterModule(const uuids::uuid &uuid, Model *model) {
        module_map_[uuid] = model;
        if (const auto it = module_resolver_map_.find(uuid);
            it != module_resolver_map_.end()) {
            for (const auto &resolver : it->second)
                resolver(model);
        }
    }

    void RegisterModule(const std::string &uuid_str, Model *model) {
        RegisterModule(
            uuids::uuid::from_string(uuid_str).value_or(uuids::uuid{}), model);
    }

private:
    std::map<uuids::uuid, std::list<ModuleResolver>> module_resolver_map_;
    std::map<uuids::uuid, Model *> module_map_;

    std::vector<nlohmann::json *> json_stack_;
    std::vector<std::vector<Model *>> graph_stack_;
    int version_;
};

// RAII scope helpers.
class JsonScope {
public:
    JsonScope(Archive &archive, nlohmann::json &j) : archive_(archive) {
        archive_.PushJson(j);
    }
    ~JsonScope() { archive_.PopJson(); }
    JsonScope(const JsonScope &) = delete;
    JsonScope &operator=(const JsonScope &) = delete;

private:
    Archive &archive_;
};

class GraphScope {
public:
    GraphScope(Archive &archive, const std::vector<Model::Ptr> &modules)
        : archive_(archive) {
        archive_.PushGraph(modules);
    }
    ~GraphScope() { archive_.PopGraph(); }
    GraphScope(const GraphScope &) = delete;
    GraphScope &operator=(const GraphScope &) = delete;

private:
    Archive &archive_;
};

} // namespace augr