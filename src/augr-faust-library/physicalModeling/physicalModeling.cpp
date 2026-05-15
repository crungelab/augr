#include "../library.h"

#include "frenchbell_dsp.h"
#include "brass_midi_dsp.h"
#include "clarinet_midi_dsp.h"


using namespace augr;

// Physical Modeling

class FrenchBellDspImpl : public FrenchBellDsp {
public:
    REFLECT_ENABLE(FaustDsp)
};
DEFINE_MODULE(FrenchBellDspImpl, "French Bell", "Instrument")

class BrassMidiDspImpl : public BrassMidiDsp {
public:
    REFLECT_ENABLE(FaustDsp)
};
DEFINE_MODULE(BrassMidiDspImpl, "Brass MIDI", "Instrument")

class ClarinetMidiDspImpl : public ClarinetMidiDsp {
public:
    REFLECT_ENABLE(FaustDsp)
};
DEFINE_MODULE(ClarinetMidiDspImpl, "Clarinet MIDI", "Instrument")

void InitFaustDspLibrary_PhysicalModeling() {
    REGISTER_MODULE(FrenchBellDspImpl);
    REGISTER_MODULE(BrassMidiDspImpl);
    REGISTER_MODULE(ClarinetMidiDspImpl);
}
