#include <augr/core/model_manufacturer.h>
#include <augr/core/archiver_manufacturer.h>

#include <augr/rack/archiver/module_archiver.h>

#include <augr/fm/operator.h>
#include <augr/fm/envelope.h>
#include <augr/fm/dexie.h>
#include <augr/fm/dexie_voice.h>

using namespace augr;
using namespace augr::fm;

DEFINE_MODULE(Operator, "Operator", "Fm")
DEFINE_MODULE(Envelope, "Envelope", "Fm")
DEFINE_MODULE(Dexie, "Dexie", "Fm")

void InitAugrFmLibrary() {
    REGISTER_MODULE(Operator);
    REGISTER_MODULE(Envelope);
    REGISTER_MODULE(Dexie);

    REGISTER_MODEL_FACTORY(DexieVoice);
}
