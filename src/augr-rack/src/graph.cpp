#include <uuid.h> // stduuid header

#include <augr/rack/graph.h>
#include <augr/rack/node.h>
#include <augr/rack/pin.h>
#include <augr/rack/wire.h>

namespace augr {

// File-scope generator. Threading: only the audio thread and the UI
// thread should ever construct subracks, and never simultaneously, so
// a single non-thread-safe generator is fine. If that assumption ever
// changes, wrap in a mutex.
namespace {
std::mt19937 &uuid_generator() {
    static std::mt19937 gen([]() {
        std::random_device rd;
        std::array<int, std::mt19937::state_size> seed_data;
        std::generate(seed_data.begin(), seed_data.end(), std::ref(rd));
        std::seed_seq seq(seed_data.begin(), seed_data.end());
        return std::mt19937(seq);
    }());
    return gen;
}

std::string GenerateUuid() {
    uuids::uuid_random_generator gen{uuid_generator()};
    return uuids::to_string(gen());
}
} // namespace

const std::string &Graph::uuid() const {
    if (uuid_.empty()) {
        uuid_ = GenerateUuid();
    }
    return uuid_;
}

void Graph::RegenerateUuid() { uuid_ = GenerateUuid(); }

void Graph::MapOutput(Pin &output) { output_map_[output.id_] = &output; }

void Graph::MapInput(Pin &input) { input_map_[input.id_] = &input; }

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

bool Graph::IsOutput(int id) const {
    return output_map_.find(id) != output_map_.end();
}

bool Graph::IsInput(int id) const {
    return input_map_.find(id) != input_map_.end();
}

void Graph::OnAddingChild(Model &model) {
    Model::OnAddingChild(model);
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