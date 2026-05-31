#include <augr/rack/module/module.h>

namespace augr {

void Module::Create() {
    Node::Create();

    CreateControls();
    CreatePins();
}

} // namespace augr