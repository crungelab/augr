#include <augr/core/model_manufacturer.h>
#include <augr/core/model.h>

#include <augr/rack/library/mixer_module.h>

using namespace augr;

DEFINE_MODEL_FACTORY(MixerModule, "MixerModule", "Rack")

void InitAugrRackLibrary() {
    REGISTER_MODEL_FACTORY(MixerModule);
}
