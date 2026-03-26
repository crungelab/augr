#pragma once

#include <augr/core/rack/graph.h>

namespace augr {

class Module;

class Rack : public Graph {
public:
    Rack() { singleton_ = this; }
    virtual ~Rack() {}
    //
    void AddModule(Module &m) { modules_.push_back(&m); }
    void RebuildExecutionOrder();
    // Accessors
    static Rack &singleton() { return *singleton_; }
    // Data members
    static Rack *singleton_;
    std::vector<Module *> modules_;
    std::vector<Module *> sorted_modules_; // cached execution order

    REFLECT_ENABLE(Graph)
};

} // namespace augr