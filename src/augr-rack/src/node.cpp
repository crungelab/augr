#include <augr/rack/node.h>
#include <augr/rack/graph.h>

namespace augr {

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