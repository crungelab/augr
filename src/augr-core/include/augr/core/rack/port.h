#pragma once

#include <vector>

#include <augr/core/rack/connector.h>

namespace augr {

class Port {
public:
  void AddConnector(Connector& connector) {
    connectors_.push_back(&connector);
  }
  virtual void CreateConnector(std::string name) {}
  //Accessors
  int nConnectors() { return connectors_.size();}
  //Data members
  std::vector<Connector*> connectors_;
};

class Inport : public Port {
public:
};

class Outport : public Port {
public:
};

} // namespace augr