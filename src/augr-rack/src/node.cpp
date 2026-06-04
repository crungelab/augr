#include <uuid.h> // stduuid header

#include <augr/rack/node.h>
#include <augr/rack/graph.h>

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

const std::string &Node::uuid() const {
    if (uuid_.empty()) {
        uuid_ = GenerateUuid();
    }
    return uuid_;
}

void Node::RegenerateUuid() { uuid_ = GenerateUuid(); }

Node::Node(Graph &graph) : Model(graph) {}

void Node::AddInput(Pin &input) {
    input.node_ = this;
    inport_.AddPin(input);
    //graph().MapInput(input);
    if(parent_) {
        graph().MapInput(input);
    }
}

void Node::AddOutput(Pin &output) {
    output.node_ = this;
    outport_.AddPin(output);
    //graph().MapOutput(output);
    if(parent_) {
        graph().MapOutput(output);
    }
}

} // namespace augr