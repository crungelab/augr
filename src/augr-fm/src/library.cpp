#include <augr/core/model_manufacturer.h>

#include <augr/fm/operator.h>
#include <augr/fm/envelope.h>
#include <augr/fm/voice.h>
#include <augr/fm/voice_bank.h>

using namespace augr;
using namespace augr::fm;

using Voice2 = Voice<2>;
using Voice4 = Voice<4>;

using VoiceBank8 = VoiceBank<8, 6>;
using VoiceBank16 = VoiceBank<16, 6>;

DEFINE_MODEL_FACTORY(Operator, "Operator", "Fm")
DEFINE_MODEL_FACTORY(Envelope, "Envelope", "Fm")
DEFINE_MODEL_FACTORY(Voice2, "2-Op Voice", "Fm")
DEFINE_MODEL_FACTORY(Voice4, "4-Op Voice", "Fm")
DEFINE_MODEL_FACTORY(VoiceBank8, "8-Voice Bank", "Fm")
DEFINE_MODEL_FACTORY(VoiceBank16, "16-Voice Bank", "Fm")

void InitAugrFmLibrary() {
    REGISTER_MODEL_FACTORY(Operator);
    REGISTER_MODEL_FACTORY(Envelope);
    REGISTER_MODEL_FACTORY(Voice2);
    REGISTER_MODEL_FACTORY(Voice4);
    REGISTER_MODEL_FACTORY(VoiceBank8);
    REGISTER_MODEL_FACTORY(VoiceBank16);
}
