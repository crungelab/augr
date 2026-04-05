#pragma once

#include <list>
#include <map>

#include <augr/core/rack/model.h>

namespace augr {

class Wire;
class Connector;

class Graph : public Model {
public:
    Graph() {}
    //
    void Connect(Connector &output, Connector &input);
    void Disconnect(Wire &wire);
    void AddOutput(Connector &output);
    void AddInput(Connector &input);
    //
    bool IsOutput(int id) const;
    bool IsInput(int id) const;

protected:
    void OnRemovingChild(Model &model) override;

public:
    // Accessors
    // Data members
    std::list<Wire *> wires_;
    std::map<int, Wire *> wire_map_;
    std::map<int, Connector *> output_map_;
    std::map<int, Connector *> input_map_;

    bool graph_dirty_ = false;

    REFLECT_ENABLE(Model)
};

} // namespace augr