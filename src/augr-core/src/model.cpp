#include <uuid.h> // stduuid header

#include <augr/core/model.h>

#include <augr/core/model_registry.h>

namespace augr {

int Model::next_id_ = 0;

Model::~Model() {
    if (!uuid_.is_nil())
        ModelRegistry::singleton().Deregister(uuid_);
}

void Model::OnCreateFresh() { uuid_ = ModelRegistry::singleton().Register(this); }

void Model::OnCreateLoaded() {
    //ModelRegistry::singleton().RegisterWithUuid(this, uuid_);
}

} // namespace augr