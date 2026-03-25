#pragma once

#include <augr/rack/graph.h>

namespace augr {

class Module;

class Rack : public Graph
{
public:
  Rack() {
    instance_ = this;
  }
  virtual ~Rack() {}
  //
  void AddModule(Module& m) {
    modules_.push_back(&m);
  }
  //Accessors
  static Rack& instance() { return *instance_; }
  //Data members
  static Rack* instance_;
  std::vector<Module*> modules_;

  REFLECT_ENABLE(Graph)
};

} // namespace augr