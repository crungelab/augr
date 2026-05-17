#include <augr/rack/pin.h>
#include <augr/rack/wire.h>

namespace augr {

int Wire::next_id_ = 0;

Wire::Wire(Pin &output, Pin &input) : output_(&output), input_(&input) {
    id_ = next_id_++;
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