#include <augr/core/model_manufacturer.h>
#include <augr/core/archiver_manufacturer.h>

#include <augr/rack/library/mixer_module.h>
#include <augr/rack/library/scope_module.h>
#include <augr/rack/library/spectral_module.h>

#include <augr/rack/archiver/module_archiver.h>
#include <augr/rack/archiver/rack_archiver.h>

using namespace augr;

DEFINE_MODULE(MixerModule, "MixerModule", "Rack")
DEFINE_MODULE(ScopeModule, "ScopeModule", "Rack")
DEFINE_MODULE(SpectralModule, "SpectralModule", "Rack")

void InitAugrRackLibrary() {
    REGISTER_MODULE(AudioInputDevice);
    REGISTER_MODULE(AudioOutputDevice);
    REGISTER_MODULE(MidiInputDevice);
    REGISTER_MODULE(MidiOutputDevice);

    REGISTER_MODULE(MixerModule);
    REGISTER_MODULE(ScopeModule);
    REGISTER_MODULE(SpectralModule);
}
