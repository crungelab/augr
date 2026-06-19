#include <augr/rack/module/module.h>

namespace augr {

void Module::OnCreate() {
    Node::OnCreate();

    CreatePins();
    CreateControls();
}

} // namespace augr