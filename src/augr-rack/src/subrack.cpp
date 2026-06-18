#include <queue>
#include <unordered_map>
#include <unordered_set>

#include <augr/archiver_factory.h>
#include <augr/model_factory.h>

#include <augr/rack/archiver/module_archiver.h>
#include <augr/rack/module/io.h>
#include <augr/rack/rack.h>
#include <augr/rack/subrack.h>
#include <augr/rack/wire.h>

namespace augr {

// ---------------------------------------------------------------------------
// Execution
// ---------------------------------------------------------------------------

void Subrack::Process() {
    Graph::Process();

    for (Module *m : sorted_modules_)
        m->Process();
}

void Subrack::RebuildExecutionOrder() {
    std::unordered_map<Module *, std::vector<Module *>> dependents;
    std::unordered_map<Module *, int> in_degree;

    // Initialize all modules
    for (Module *m : modules_) {
        in_degree[m] = 0;
    }

    // Build graph: m -> downstream.
    // Deduplicate edges so multiple wires between the same two modules
    // do not inflate in_degree.
    for (Module *m : modules_) {
        std::unordered_set<Module *> seen_downstream;

        for (auto *out_pin : m->outport_.pins_) {
            for (auto *out_wire : out_pin->wires_) {
                if (!out_wire || !out_wire->input_ ||
                    !out_wire->input_->node_) {
                    continue;
                }

                Module *downstream =
                    dynamic_cast<Module *>(out_wire->input_->node_);
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
}

// ---------------------------------------------------------------------------
// Child management
// ---------------------------------------------------------------------------

void Subrack::OnAddingChild(Model &model) {
    Graph::OnAddingChild(model);

    if (auto *m = dynamic_cast<Module *>(&model)) {
        OnAddingModule(*m);
    }
    if (auto *d = dynamic_cast<Io *>(&model)) {
        OnAddingIo(*d);
    }
}

void Subrack::OnRemovingChild(Model &model) {
    if (auto *m = dynamic_cast<Module *>(&model)) {
        OnRemovingModule(*m);
    }
    if (auto *d = dynamic_cast<Io *>(&model)) {
        OnRemovingIo(*d);
    }
    Graph::OnRemovingChild(model);
}

void Subrack::OnAddingModule(Module &module) {
    AddModule(module);
    topology_changed_ = true;
}

void Subrack::OnRemovingModule(Module &module) {
    RemoveModule(module);
    topology_changed_ = true;
}
// ---------------------------------------------------------------------------
// Outer-rack lookup
// ---------------------------------------------------------------------------

Rack *Subrack::OuterRack() {
    if (outer_rack_)
        return outer_rack_;
    for (auto p = shared_from_this(); p != nullptr; p = p->parent_.lock()) {
        if (auto *r = dynamic_cast<Rack *>(p.get())) {
            outer_rack_ = r;
            return r;
        }
    }
    return nullptr;
}

// ---------------------------------------------------------------------------
// Action queue forwarding
// ---------------------------------------------------------------------------

void Subrack::EnqueueAction(std::function<void()> action) {
    if (Rack *r = OuterRack()) {
        r->EnqueueAction(std::move(action));
    }
    // If there's no outer rack, the subrack is standalone and has no
    // thread to bridge to. Drop the action silently. (Could assert here
    // instead if standalone usage should never enqueue.)
}

} // namespace augr

using namespace augr;
DEFINE_MODEL_FACTORY(Subrack, "Subrack", "General")
