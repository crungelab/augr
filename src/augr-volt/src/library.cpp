#include <augr/core/model_manufacturer.h>
#include <augr/core/model.h>

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

DEFINE_MODEL_FACTORY(CvModule, "CvModule", "Volt")
DEFINE_MODEL_FACTORY(MidiCvModule, "MidiCvModule", "Volt")
DEFINE_MODEL_FACTORY(VcoModule, "VcoModule", "Volt")
DEFINE_MODEL_FACTORY(VcaModule, "VcaModule", "Volt")
DEFINE_MODEL_FACTORY(AdsrModule, "AdsrModule", "Volt")
DEFINE_MODEL_FACTORY(VcfModule, "VcfModule", "Volt")
DEFINE_MODEL_FACTORY(AttenuverterModule, "AttenuverterModule", "Volt")
DEFINE_MODEL_FACTORY(LfoModule, "LfoModule", "Volt")
DEFINE_MODEL_FACTORY(NoiseModule, "NoiseModule", "Volt")

void InitAugrVoltLibrary() {
    REGISTER_MODEL_FACTORY(CvModule);
    REGISTER_MODEL_FACTORY(MidiCvModule);
    REGISTER_MODEL_FACTORY(VcoModule);
    REGISTER_MODEL_FACTORY(VcaModule);
    REGISTER_MODEL_FACTORY(AdsrModule);
    REGISTER_MODEL_FACTORY(VcfModule);
    REGISTER_MODEL_FACTORY(AttenuverterModule);
    REGISTER_MODEL_FACTORY(LfoModule);
    REGISTER_MODEL_FACTORY(NoiseModule);
}
