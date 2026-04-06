#include <augr/core/rack/node.h>
#include <augr/core/rack/graph.h>

namespace augr {

Node::Node(Graph &graph) : Model(graph) {}

void Node::AddInput(Connector &input) {
    inport_.AddConnector(input);
    graph().AddInput(input);
}

void Node::AddOutput(Connector &output) {
    outport_.AddConnector(output);
    graph().AddOutput(output);
}

} // namespace augr