#include <augr/core/rack/connector.h>
#include <augr/core/rack/wire.h>

namespace augr {

Wire::Wire(Connector &output, Connector &input) : output_(&output), input_(&input) {
    output.AddWire(*this);
    input.AddWire(*this);

    connection_ = input.Connect(output);
}

Wire::~Wire() {
    if (input_) {
        input_->Disconnect(*connection_);
        input_->RemoveWire(*this);
        input_ = nullptr;
    }

    if (output_) {
        output_->RemoveWire(*this);
        output_ = nullptr;
    }
}
} // namespace augr