#include <queue>
#include <unordered_set>

#include <augr/core/rack/module/module.h>
#include <augr/core/rack/rack.h>
#include <augr/core/rack/wire.h>

namespace augr {

Rack *Rack::singleton_;

void Rack::EnqueueAction(std::function<void()> action,
                         std::function<void()> update_action) {
    std::lock_guard lock(mutex_);
    pending_actions_.push_back(std::move(action));
    if (update_action) {
        pending_update_actions_.push_back(std::move(update_action));
    }
}

void Rack::EnqueueUpdateAction(std::function<void()> action) {
    std::lock_guard lock(mutex_);
    pending_update_actions_.push_back(std::move(action));
}

void Rack::ProcessActions() {
    std::vector<std::function<void()>> actions;
    {
        std::lock_guard lock(mutex_);
        std::swap(actions, pending_actions_);
    }
    for (auto &a : actions) a();
}

void Rack::ProcessUpdateActions() {
    std::vector<std::function<void()>> actions;
    {
        std::lock_guard lock(mutex_);
        std::swap(actions, pending_update_actions_);
    }
    for (auto &a : actions) a();
}

void Rack::RebuildExecutionOrder() {
    std::unordered_map<Module *, std::vector<Module *>> dependents;
    std::unordered_map<Module *, int> in_degree;

    // Initialize all modules
    for (Module *m : modules_) {
        in_degree[m] = 0;
    }

    // Build graph: m -> downstream
    // Deduplicate edges so multiple wires between the same two modules
    // do not inflate in_degree.
    for (Module *m : modules_) {
        std::unordered_set<Module *> seen_downstream;

        for (auto *out_connector : m->outport_.connectors_) {
            for (auto *out_wire : out_connector->wires_) {
                if (!out_wire || !out_wire->input_ ||
                    !out_wire->input_->owner_) {
                    continue;
                }

                Module *downstream =
                    dynamic_cast<Module *>(out_wire->input_->owner_);
                if (!downstream || downstream == m) {
                    continue;
                }

                if (seen_downstream.insert(downstream).second) {
                    dependents[m].push_back(downstream);
                    ++in_degree[downstream];
                }
            }
        }
    }

    // Seed with every module that has no upstream dependencies.
    std::queue<Module *> ready;
    for (Module *m : modules_) {
        if (in_degree[m] == 0) {
            ready.push(m);
        }
    }

    sorted_modules_.clear();
    sorted_modules_.reserve(modules_.size());

    while (!ready.empty()) {
        Module *m = ready.front();
        ready.pop();

        sorted_modules_.push_back(m);

        auto it = dependents.find(m);
        if (it == dependents.end()) {
            continue;
        }

        for (Module *dep : it->second) {
            if (--in_degree[dep] == 0) {
                ready.push(dep);
            }
        }
    }

    // Cycle handling:
    // If anything remains, those modules are part of a cycle
    // or are otherwise unreachable by Kahn's algorithm.
    if (sorted_modules_.size() != modules_.size()) {
        std::unordered_set<Module *> already_sorted(sorted_modules_.begin(),
                                                    sorted_modules_.end());

        for (Module *m : modules_) {
            if (!already_sorted.contains(m)) {
                sorted_modules_.push_back(m);
            }
        }

        // TODO: log/report cycle detected here
    }

    graph_dirty_ = false;
}

} // namespace augr