#include <augr/core/model_manufacturer.h>
#include <augr/core/archiver_manufacturer.h>

#include <augr/rack/subrack.h>

#include <augr/rack/archiver/module_archiver.h>
#include <augr/rack/archiver/rack_archiver.h>
#include <augr/rack/archiver/subrack_archiver.h>
#include <augr/rack/archiver/voice_archiver.h>
#include <augr/rack/archiver/voicebank_archiver.h>

#include <augr/rack/library/probe_module.h>
#include <augr/rack/library/scope_module.h>
#include <augr/rack/library/spectral_module.h>
#include <augr/rack/library/mixer_module.h>

using namespace augr;

DEFINE_MODULE(ProbeModule, "ProbeModule", "Rack")
DEFINE_MODULE(ScopeModule, "ScopeModule", "Rack")
DEFINE_MODULE(SpectralModule, "SpectralModule", "Rack")
DEFINE_MODULE(MixerModule, "MixerModule", "Rack")

void InitAugrRackLibrary() {
    REGISTER_MODULE(AudioInputDevice);
    REGISTER_MODULE(AudioOutputDevice);
    REGISTER_MODULE(MidiInputDevice);
    REGISTER_MODULE(MidiOutputDevice);

    REGISTER_MODULE(AudioInputModule);
    REGISTER_MODULE(AudioOutputModule);
    REGISTER_MODULE(MidiInputModule);
    REGISTER_MODULE(MidiOutputModule);
    REGISTER_MODULE(CvInputModule);
    REGISTER_MODULE(CvOutputModule);

    REGISTER_MODEL_FACTORY(Subrack);
    REGISTER_ARCHIVER_FACTORY(SubrackArchiver)

    REGISTER_MODEL_FACTORY(Voice);
    REGISTER_ARCHIVER_FACTORY(VoiceArchiver)

    REGISTER_MODEL_FACTORY(Voicebank);
    REGISTER_ARCHIVER_FACTORY(VoicebankArchiver)

    REGISTER_MODULE(ProbeModule);
    REGISTER_MODULE(ScopeModule);
    REGISTER_MODULE(SpectralModule);
    REGISTER_MODULE(MixerModule);
}
