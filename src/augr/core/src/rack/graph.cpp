#include <augr/core/rack/graph.h>
#include <augr/core/rack/node.h>
#include <augr/core/rack/pin.h>
#include <augr/core/rack/wire.h>

namespace augr {

void Graph::AddOutput(Pin &output) { output_map_[output.id_] = &output; }

void Graph::AddInput(Pin &input) { input_map_[input.id_] = &input; }

void Graph::Connect(Pin &output, Pin &input) {
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

bool Graph::IsOutputPin(int pin_id) const {
    return output_map_.find(pin_id) != output_map_.end();
}

bool Graph::IsInputPin(int pin_id) const {
    return input_map_.find(pin_id) != input_map_.end();
}

void Graph::OnRemovingChild(Model &model) {
    auto node = dynamic_cast<Node *>(&model);
    for (auto pin : node->inport_.pins_) {
        for (auto wire : pin->wires_) {
            Disconnect(*wire);
        }
    }
    for (auto pin : node->outport_.pins_) {
        for (auto wire : pin->wires_) {
            Disconnect(*wire);
        }
    }
}
} // namespace augr