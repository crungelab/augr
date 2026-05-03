#include <augr/rack/module/module.h>

namespace augr {

bool Module::Create(Part &owner) {
    Node::Create(owner);

    CreateControls();
    CreatePins();

    return true;
}

} // namespace augr