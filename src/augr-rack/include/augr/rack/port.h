#pragma once

#include <vector>

#include <augr/rack/pin.h>

namespace augr {

class Port {
public:
  void AddPin(Pin& pin) {
    pins_.push_back(&pin);
  }
  virtual void CreatePin(std::string name) {}
  //Accessors
  int nPins() { return pins_.size();}
  //Data members
  std::vector<Pin*> pins_;
};

class Inport : public Port {
public:
};

class Outport : public Port {
public:
};

} // namespace augr