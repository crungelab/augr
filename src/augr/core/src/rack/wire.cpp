#include <augr/core/rack/pin.h>
#include <augr/core/rack/wire.h>

namespace augr {

Wire::Wire(Pin &output, Pin &input) : output_(&output), input_(&input) {
    input.AddWire(*this);
    output.AddWire(*this);

    subscription_ = output.Connect(input);
}

Wire::~Wire() {
    if (output_) {
        output_->Disconnect(*input_);
        output_->RemoveWire(*this);
        output_ = nullptr;
    }
    if (input_) {
        input_->RemoveWire(*this);
        input_ = nullptr;
    }
}
} // namespace augr