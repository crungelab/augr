#pragma once

#include <augr/core/part.h>

namespace augr {

class Pin;
class Connection;

class Wire : public Part {
public:
  Wire(Pin& output, Pin& input);
  ~Wire();
  //Data members
  Pin* output_ = nullptr;
  Pin* input_ = nullptr;
  Connection* connection_ = nullptr;
};

} // namespace augr