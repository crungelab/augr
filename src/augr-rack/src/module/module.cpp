#include <augr/rack/module/module.h>

namespace augr {

void Module::OnCreate() {
    Node::OnCreate();

    CreateControls();
    CreatePins();
}

} // namespace augr