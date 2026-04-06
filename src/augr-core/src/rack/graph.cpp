#include <augr/core/rack/graph.h>
#include <augr/core/rack/node.h>
#include <augr/core/rack/connector.h>
#include <augr/core/rack/wire.h>

namespace augr {

void Graph::AddOutput(Connector &output) { output_map_[output.id_] = &output; }

void Graph::AddInput(Connector &input) { input_map_[input.id_] = &input; }

void Graph::Connect(Connector &output, Connector &input) {
    auto wire = new Wire(output, input);
    wires_.push_back(wire);
    wire_map_[wire->id_] = wire;
    graph_dirty_ = true;
}

void Graph::Disconnect(Wire &wire) {
    wires_.remove(&wire);
    wire_map_.erase(wire.id_);
    delete &wire;
    graph_dirty_ = true;
}

bool Graph::IsOutput(int id) const {
    return output_map_.find(id) != output_map_.end();
}

bool Graph::IsInput(int id) const {
    return input_map_.find(id) != input_map_.end();
}

void Graph::OnRemovingChild(Model &model) {
    auto node = dynamic_cast<Node *>(&model);
    for (auto connector : node->inport_.connectors_) {
        for (auto wire : connector->wires_) {
            Disconnect(*wire);
        }
    }
    for (auto connector : node->outport_.connectors_) {
        for (auto wire : connector->wires_) {
            Disconnect(*wire);
        }
    }
}
} // namespace augr