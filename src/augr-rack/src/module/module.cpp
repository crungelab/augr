#include <augr/rack/module/module.h>

namespace augr {

void Module::Create(Model *parent) {
    Node::Create(parent);

    CreateControls();
    CreatePins();
}

} // namespace augr