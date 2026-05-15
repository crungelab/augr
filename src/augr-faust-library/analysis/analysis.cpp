#include "../library.h"

#include "dbmeter_dsp.h"
#include "spectral_level_dsp.h"
#include "vumeter_dsp.h"


using namespace augr;

// Analysis

class DbMeterDspImpl : public DbMeterDsp {
public:
    REFLECT_ENABLE(FaustDsp)
};
DEFINE_MODULE(DbMeterDspImpl, "Db Meter", "Analysis")

class SpectralLevelDspImpl : public SpectralLevelDsp {
public:
    REFLECT_ENABLE(FaustDsp)
};
DEFINE_MODULE(SpectralLevelDspImpl, "Spectral Level", "Analysis")

class VuMeterDspImpl : public VuMeterDsp {
public:
    REFLECT_ENABLE(FaustDsp)
};
DEFINE_MODULE(VuMeterDspImpl, "Vu Meter", "Analysis")

void InitFaustDspLibrary_Analysis() {
    REGISTER_MODULE(DbMeterDspImpl);
    REGISTER_MODULE(SpectralLevelDspImpl);
    REGISTER_MODULE(VuMeterDspImpl);
}