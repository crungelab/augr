#pragma once

#include <list>
#include <map>

#include <augr/core/model.h>

namespace augr {

class Wire;
class Pin;

class Graph : public Model {
public:
    Graph() {}
    //
    void Connect(Pin &output, Pin &input);
    void Disconnect(Wire &wire);
    void AddOutput(Pin &output);
    void AddInput(Pin &input);
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
    std::map<int, Pin *> output_map_;
    std::map<int, Pin *> input_map_;

    bool graph_dirty_ = false;

    REFLECT_ENABLE(Model)
};

} // namespace augr