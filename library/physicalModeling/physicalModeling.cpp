#include "../library.h"

#include "frenchbell_dsp.h"

using namespace augr;

// Physical Modeling

class FrenchBellDspImpl : public FrenchBellDsp {
public:
    REFLECT_ENABLE(FaustDsp)
};
DEFINE_MODEL_FACTORY(FrenchBellDspImpl, "French Bell", "Instrument")


void InitFaustDspLibrary_PhysicalModeling() {
    REGISTER_MODEL_FACTORY(FrenchBellDspImpl);
}
