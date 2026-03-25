#include <augr/rack/rack.h>

#include "module.h"

namespace augr {

bool Module::Create(Part& owner) {
  Node::Create(owner);
  Rack::instance().AddModule(*this);
  return true;
}

} // namespace augr