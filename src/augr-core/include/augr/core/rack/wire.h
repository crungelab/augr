#pragma once

#include <augr/core/part.h>

namespace augr {

class Connector;
class Connection;

class Wire : public Part {
public:
  Wire(Connector& output, Connector& input);
  ~Wire();
  //Data members
  Connector* output_ = nullptr;
  Connector* input_ = nullptr;
  Connection* connection_ = nullptr;
};

} // namespace augr