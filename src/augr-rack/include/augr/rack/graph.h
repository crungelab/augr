#pragma once

#include <list>
#include <map>

#include <augr/rack/module/module.h>

namespace augr {

class Wire;
class Pin;

class Graph : public Module {
public:
    Graph() {}
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

    //
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

    bool graph_dirty_ = false;

    REFLECT_ENABLE(Module)
};

} // namespace augr