#pragma once

#include <list>
#include <map>

#include <sigslot/signal.hpp>

#include <augr/rack/module/module.h>

namespace augr {

class Wire;
class Pin;

class Graph : public Module {
public:
    Graph() {}

    void Process() override;

    void Connect(Pin &output, Pin &input);
    void Disconnect(Wire &wire);
    void MapOutput(Pin &output);
    void MapInput(Pin &input);
    //
    bool IsOutput(int id) const;
    bool IsInput(int id) const;

protected:
    void OnAddingChild(Model &model) override;
    void OnRemovingChild(Model &model) override;
    virtual void OnTopologyChanged() { on_topology_changed(); }

public:
    // Accessors
    // Data members --- identity
    // Mutable to allow lazy initialization from const uuid() accessor.
    mutable std::string uuid_;

    // Data members
    std::list<Wire *> wires_;
    std::map<int, Wire *> wire_map_;
    std::map<int, Pin *> output_map_;
    std::map<int, Pin *> input_map_;

    bool topology_changed_ = false;
    sigslot::signal_st<> on_topology_changed;

    REFLECT_ENABLE(Module)
};

} // namespace augr