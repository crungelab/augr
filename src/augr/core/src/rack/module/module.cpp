#include <augr/core/rack/rack.h>

#include <augr/core/rack/module/module.h>

namespace augr {

bool Module::Create(Part& owner) {
  Node::Create(owner);
  Rack::instance().AddModule(*this);
  return true;
}

} // namespace augr