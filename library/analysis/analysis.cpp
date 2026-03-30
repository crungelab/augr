#include "../library.h"

#include "dbmeter_dsp.h"
#include "vumeter_dsp.h"


using namespace augr;

// Analysis

class DbMeterDspImpl : public DbMeterDsp {
public:
    REFLECT_ENABLE(FaustDsp)
};
DEFINE_MODEL_FACTORY(DbMeterDspImpl, "Db Meter", "Analysis")

class VuMeterDspImpl : public VuMeterDsp {
public:
    REFLECT_ENABLE(FaustDsp)
};
DEFINE_MODEL_FACTORY(VuMeterDspImpl, "Vu Meter", "Analysis")

void InitFaustDspLibrary_Analysis() {
    REGISTER_MODEL_FACTORY(DbMeterDspImpl);
    REGISTER_MODEL_FACTORY(VuMeterDspImpl);
}