#include <augr/rack/module/module.h>

namespace augr {

void Module::OnCreate() {
    Node::OnCreate();

    console_ = std::make_shared<Model>();

    CreatePins();
    CreateControls();
}

} // namespace augr