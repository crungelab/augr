#pragma once

#include <algorithm>
#include <functional>
#include <string>
#include <vector>

#include <augr/core/midi/midi_message.h>

#include <augr/rack/graph.h>

namespace augr {

class Module;
class Rack;

class Subrack : public Graph {
public:
    Subrack() = default;

    void Create(Model *parent) override {
        Graph::Create(parent);
        label_ = "Subrack";
    }

    // -- Identity -------------------------------------------------------
    // Stable identifier that survives save/load and uniquely identifies
    // this subrack within the project. Generated lazily on first call;
    // serialized as part of the subrack's JSON. Used by view-state
    // persistence, undo history, and anything else that needs to refer
    // to a specific subrack across sessions.
    const std::string &uuid() const;

    // Reset to a fresh uuid. Called on paste so that the new copy
    // doesn't collide with the original's identity.
    void RegenerateUuid();

    // For deserialization: set the uuid directly without generating.
    void set_uuid(std::string uuid) { uuid_ = std::move(uuid); }

    // -- Execution ------------------------------------------------------
    // Rebuilds execution order if dirty, then walks sorted_modules_
    // in topological order calling Process() on each.
    void Process() override;

    // Topological sort over modules_ using outport wires.
    // Non-Module Nodes (e.g. WDF ports) participate in topology
    // but are not scheduled here.
    void RebuildExecutionOrder();

    // -- Child management ----------------------------------------------
    // Adds Module / Device branches on top of Graph's wire+pin bookkeeping.
    void OnAddingChild(Model &model) override;
    void OnRemovingChild(Model &model) override;

    // -- Action queue interface ----------------------------------------
    // These walk up parents to the outer Rack and forward.
    // Defined in subrack.cpp to avoid including rack.h here.
    void EnqueueAction(std::function<void()> action,
                       std::function<void()> update_action = nullptr);
    void EnqueueUpdateAction(std::function<void()> action);

    // -- Accessors -----------------------------------------------------
    // Walks parents to find the enclosing Rack (returns nullptr if none).
    Rack *OuterRack();

    // Data members --- scheduling
    std::vector<Module *> modules_;
    std::vector<Module *> sorted_modules_; // cached execution order

    // Data members --- identity
    // Mutable to allow lazy initialization from const uuid() accessor.
    mutable std::string uuid_;

    REFLECT_ENABLE(Graph)

private:
    void AddModule(Module &m) { modules_.push_back(&m); }
    void RemoveModule(Module &m) {
        modules_.erase(std::remove(modules_.begin(), modules_.end(), &m),
                       modules_.end());
    }
};

} // namespace augr