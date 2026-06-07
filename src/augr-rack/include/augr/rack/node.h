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
    const std::string &uuid() const;
    void RegenerateUuid();
    void set_uuid(std::string uuid) { uuid_ = std::move(uuid); }

    // Pins
    void AddInput(Pin &input);
    void AddOutput(Pin &output);

    // Accessors
    Graph &graph();
    const Graph &graph() const;


    // Data members
    mutable std::string uuid_;
    Inport inport_;
    Outport outport_;

    REFLECT_ENABLE(Model)
};

} // namespace augr