#include <augr/core/rack/node.h>
#include <augr/core/rack/graph.h>

namespace augr {

Node::Node(Graph &graph) : Model(graph) {}

void Node::AddInput(Pin &input) {
    inport_.AddPin(input);
    graph().AddInput(input);
}

void Node::AddOutput(Pin &output) {
    outport_.AddPin(output);
    graph().AddOutput(output);
}

} // namespace augr