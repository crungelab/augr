#include <augr/core/model_manufacturer.h>

#include <augr/fm/operator.h>
#include <augr/fm/envelope.h>
#include <augr/fm/voice.h>

using namespace augr;
using namespace augr::fm;

using Voice2 = Voice<2>;
using Voice4 = Voice<4>;

DEFINE_MODEL_FACTORY(Operator, "Operator", "Fm")
DEFINE_MODEL_FACTORY(Envelope, "Envelope", "Fm")
DEFINE_MODEL_FACTORY(Voice2, "2-Op Voice", "Fm")
DEFINE_MODEL_FACTORY(Voice4, "4-Op Voice", "Fm")

void InitAugrFmLibrary() {
    REGISTER_MODEL_FACTORY(Operator);
    REGISTER_MODEL_FACTORY(Envelope);
    REGISTER_MODEL_FACTORY(Voice2);
    REGISTER_MODEL_FACTORY(Voice4);
}
