#pragma once

#include <augr/core/rack/graph.h>

namespace augr {

class Module;

class Rack : public Graph {
public:
    Rack() { instance_ = this; }
    virtual ~Rack() {}
    //
    void AddModule(Module &m) { modules_.push_back(&m); }
    void RebuildExecutionOrder();
    // Accessors
    static Rack &instance() { return *instance_; }
    // Data members
    static Rack *instance_;
    std::vector<Module *> modules_;
    std::vector<Module *> sorted_modules_; // cached execution order

    REFLECT_ENABLE(Graph)
};

} // namespace augr