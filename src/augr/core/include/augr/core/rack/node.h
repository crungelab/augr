#pragma once

#include <vector>

#include <augr/core/rack/model.h>
#include "port.h"

namespace augr {

class Graph;

class Node : public Model {
public:
  Node() {}
  Node(Graph& graph);
  //Pins
  void AddInput(Pin& input);
  void AddOutput(Pin& output);
  //Accessors
  Graph& graph() { return *(Graph*)owner_; }
  //Data members
  //Pins
  Inport inport_;
  Outport outport_;

  REFLECT_ENABLE(Model)
};

} // namespace augr