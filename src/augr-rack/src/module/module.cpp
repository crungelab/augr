#include <augr/rack/module/module.h>

namespace augr {

void Module::Create(Part *owner) {
    Node::Create(owner);

    CreateControls();
    CreatePins();
}

} // namespace augr