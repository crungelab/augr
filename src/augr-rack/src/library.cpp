#include <augr/core/model_manufacturer.h>

#include <augr/rack/library/mixer_module.h>
#include <augr/rack/library/scope_module.h>

using namespace augr;

DEFINE_MODEL_FACTORY(MixerModule, "MixerModule", "Rack")
DEFINE_MODEL_FACTORY(ScopeModule, "ScopeModule", "Rack")

void InitAugrRackLibrary() {
    REGISTER_MODEL_FACTORY(MixerModule);
    REGISTER_MODEL_FACTORY(ScopeModule);
}
