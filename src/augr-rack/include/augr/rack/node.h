#pragma once

#include <vector>

#include "port.h"
#include <augr/core/model.h>

namespace augr {

class Graph;

class Node : public Model {
public:
    Node() {}
    Node(Graph &graph);

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

    // Pins
    void AddInput(Pin &input);
    void AddOutput(Pin &output);
    // Accessors
    Graph &graph() { return *(Graph *)parent_; }
    const Graph &graph() const { return *(const Graph *)parent_; }
    // Data members
    // Identity
    // Mutable to allow lazy initialization from const uuid() accessor.
    mutable std::string uuid_;

    // Pins
    Inport inport_;
    Outport outport_;

    REFLECT_ENABLE(Model)
};

} // namespace augr