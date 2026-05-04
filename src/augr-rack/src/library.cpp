#include <augr/core/model_manufacturer.h>
#include <augr/core/model_factory.h>

#include <augr/rack/library/mixer_module.h>
#include <augr/rack/library/scope_module.h>
#include <augr/rack/library/spectral_module.h>

using namespace augr;

DEFINE_MODEL_FACTORY(MixerModule, "MixerModule", "Rack")
DEFINE_MODEL_FACTORY(ScopeModule, "ScopeModule", "Rack")
DEFINE_MODEL_FACTORY(SpectralModule, "SpectralModule", "Rack")

void InitAugrRackLibrary() {
    REGISTER_MODEL_FACTORY(MixerModule);
    REGISTER_MODEL_FACTORY(ScopeModule);
    REGISTER_MODEL_FACTORY(SpectralModule);
}
