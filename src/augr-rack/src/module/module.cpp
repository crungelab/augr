#include <augr/rack/rack.h>

#include <augr/rack/module/module.h>

namespace augr {

bool Module::Create(Part &owner) {
    Node::Create(owner);

    CreateControls();
    CreatePins();

    Rack::singleton().AddModule(*this);
    return true;
}

} // namespace augr