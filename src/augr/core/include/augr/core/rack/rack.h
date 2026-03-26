#pragma once

#include <functional>

#include <augr/core/rack/graph.h>

namespace augr {

class Module;

class Rack : public Graph {
public:
    Rack() { singleton_ = this; }
    virtual ~Rack() {}
    //
    void AddModule(Module &m) { modules_.push_back(&m); }

    void EnqueueAction(std::function<void()> action,
                       std::function<void()> update_action = nullptr);
    void EnqueueUpdateAction(std::function<void()> action);
    
    void ProcessActions();
    void ProcessUpdateActions();

    void RebuildExecutionOrder();
    // Accessors
    static Rack &singleton() { return *singleton_; }
    // Data members
    static Rack *singleton_;
    std::vector<Module *> modules_;
    std::vector<Module *> sorted_modules_; // cached execution order
    //
    std::mutex mutex_;
    std::vector<std::function<void()>> pending_actions_;
    std::vector<std::function<void()>> pending_update_actions_;

    REFLECT_ENABLE(Graph)
};

} // namespace augr