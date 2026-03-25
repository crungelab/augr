#include <augr/rack/pin.h>
#include <augr/rack/wire.h>

namespace augr {

Wire::Wire(Pin& output, Pin& input) : output_(&output), input_(&input)
{
  input.AddWire(*this);
  output.AddWire(*this);

  subscription_ = output.Connect(input);
}

} // namespace augr