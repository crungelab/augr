#pragma once

#include <algorithm>
#include <functional>
#include <string>
#include <vector>

#include <augr/rack/graph.h>

namespace augr {

class Module;
class Io;
class Rack;

class Subrack : public Graph {
public:
    Subrack() = default;

    void Create() override {
        Graph::Create();
        label_ = "Subrack";
    }

    // -- Execution ------------------------------------------------------
    // Rebuilds execution order if dirty, then walks sorted_modules_
    // in topological order calling Process() on each.
    void Process() override;

    void OnTopologyChanged() override {
        Graph::OnTopologyChanged();
        RebuildExecutionOrder();
    }

    // Topological sort over modules_ using outport wires.
    // Non-Module Nodes (e.g. WDF ports) participate in topology
    // but are not scheduled here.
    void RebuildExecutionOrder();

    // -- Child management ----------------------------------------------
    // Adds Module / Device branches on top of Graph's wire+pin bookkeeping.
    void OnAddingChild(Model &model) override;
    void OnRemovingChild(Model &model) override;

    virtual void OnAddingModule(Module &module);
    virtual void OnRemovingModule(Module &module);

    virtual void OnAddingIo(Io &io) {}
    virtual void OnRemovingIo(Io &io) {}

    // -- Action queue interface ----------------------------------------
    // These walk up parents to the outer Rack and forward.
    // Defined in subrack.cpp to avoid including rack.h here.
    void EnqueueAction(std::function<void()> action,
                       std::function<void()> update_action = nullptr);
    void EnqueueUpdateAction(std::function<void()> action);

    // -- Accessors -----------------------------------------------------
    // Walks parents to find the enclosing Rack (returns nullptr if none).
    Rack *OuterRack();
    Rack &rack() { return *OuterRack(); }

    // Data members --- scheduling
    Rack *outer_rack_ = nullptr; // cached pointer to enclosing Rack
    std::vector<Module *> modules_;
    std::vector<Module *> sorted_modules_; // cached execution order

    REFLECT_ENABLE(Graph)

private:
    void AddModule(Module &m) { modules_.push_back(&m); }
    void RemoveModule(Module &m) {
        modules_.erase(std::remove(modules_.begin(), modules_.end(), &m),
                       modules_.end());
    }
};

} // namespace augr