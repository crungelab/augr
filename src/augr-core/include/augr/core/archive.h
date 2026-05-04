#pragma once

#include <nlohmann/json.hpp>

#include <algorithm>
#include <typeindex>
#include <utility>
#include <vector>

namespace augr {

class Model;

class Archive {
public:
    explicit Archive(nlohmann::json &root, int version = 1)
        : version_(version) {
        json_stack_.push_back(&root);
    }

    [[nodiscard]] nlohmann::json &json() { return *json_stack_.back(); }
    [[nodiscard]] const nlohmann::json &json() const {
        return *json_stack_.back();
    }

    [[nodiscard]] int version() const { return version_; }

    void PushJson(nlohmann::json &j) { json_stack_.push_back(&j); }
    void PopJson() { json_stack_.pop_back(); }

    void PushGraph(std::vector<Model *> modules) {
        graph_stack_.push_back(std::move(modules));
    }
    void PopGraph() { graph_stack_.pop_back(); }

    [[nodiscard]] Model *ResolveModule(int index) const {
        if (graph_stack_.empty())
            return nullptr;
        const auto &current = graph_stack_.back();
        if (index < 0 || index >= static_cast<int>(current.size())) {
            return nullptr;
        }
        return current[index];
    }

    [[nodiscard]] int IndexOf(const Model *model) const {
        if (graph_stack_.empty())
            return -1;
        const auto &current = graph_stack_.back();
        auto it = std::find(current.begin(), current.end(), model);
        return it == current.end() ? -1
                                   : static_cast<int>(it - current.begin());
    }

private:
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
    GraphScope(Archive &archive, std::vector<Model *> modules)
        : archive_(archive) {
        archive_.PushGraph(std::move(modules));
    }
    ~GraphScope() { archive_.PopGraph(); }
    GraphScope(const GraphScope &) = delete;
    GraphScope &operator=(const GraphScope &) = delete;

private:
    Archive &archive_;
};

} // namespace augr