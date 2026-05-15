#include <augr/core/model_manufacturer.h>
#include <augr/core/archiver_manufacturer.h>

#include <augr/rack/archiver/module_archiver.h>

#include <augr/volt/cv_module.h>
#include <augr/volt/midi_cv_module.h>

#include <augr/volt/vco_module.h>
#include <augr/volt/vca_module.h>
#include <augr/volt/adsr_module.h>
#include <augr/volt/vcf_module.h>
#include <augr/volt/attenuverter_module.h>
#include <augr/volt/lfo_module.h>
#include <augr/volt/noise_module.h>

using namespace augr;

DEFINE_MODULE(CvModule, "CvModule", "Volt")
DEFINE_MODULE(MidiCvModule, "MidiCvModule", "Volt")
DEFINE_MODULE(VcoModule, "VcoModule", "Volt")
DEFINE_MODULE(VcaModule, "VcaModule", "Volt")
DEFINE_MODULE(AdsrModule, "AdsrModule", "Volt")
DEFINE_MODULE(VcfModule, "VcfModule", "Volt")
DEFINE_MODULE(AttenuverterModule, "AttenuverterModule", "Volt")
DEFINE_MODULE(LfoModule, "LfoModule", "Volt")
DEFINE_MODULE(NoiseModule, "NoiseModule", "Volt")

void InitAugrVoltLibrary() {
    REGISTER_MODULE(CvModule);
    REGISTER_MODULE(MidiCvModule);
    REGISTER_MODULE(VcoModule);
    REGISTER_MODULE(VcaModule);
    REGISTER_MODULE(AdsrModule);
    REGISTER_MODULE(VcfModule);
    REGISTER_MODULE(AttenuverterModule);
    REGISTER_MODULE(LfoModule);
    REGISTER_MODULE(NoiseModule);
}
