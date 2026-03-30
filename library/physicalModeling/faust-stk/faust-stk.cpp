#include "../../library.h"

#include "blow_bottle_dsp.h"

using namespace augr;

// Physical Modeling

class BlowBottleDspImpl : public BlowBottleDsp {
public:
    REFLECT_ENABLE(FaustDsp)
};
DEFINE_MODEL_FACTORY(BlowBottleDspImpl, "Blow Bottle", "Instrument")


void InitFaustDspLibrary_PhysicalModeling_FaustStk() {
    REGISTER_MODEL_FACTORY(BlowBottleDspImpl);
}
