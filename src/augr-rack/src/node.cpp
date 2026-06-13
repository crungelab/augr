#include <augr/rack/graph.h>
#include <augr/rack/node.h>

namespace augr {

Node::Node(Graph &graph) : Model(graph) {}

void Node::AddInput(Pin &input) {
    input.node_ = this;
    inport_.AddPin(input);
    graph().MapInput(input);
}

void Node::AddOutput(Pin &output) {
    output.node_ = this;
    outport_.AddPin(output);
    graph().MapOutput(output);
}

// Accessors
Graph &Node::graph() {
    auto p = std::static_pointer_cast<Graph>(parent_.lock());
    assert(p && "Node has no parent Graph");
    return *p;
}
const Graph &Node::graph() const {
    auto p = std::static_pointer_cast<Graph>(parent_.lock());
    assert(p && "Node has no parent Graph");
    return *p;
}

} // namespace augr