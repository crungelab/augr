#include <augr/core/model_manufacturer.h>
#include <augr/core/archiver_manufacturer.h>

#include <augr/rack/archiver/module_archiver.h>

#include <augr/fm/operator.h>
#include <augr/fm/envelope.h>

using namespace augr;
using namespace augr::fm;

DEFINE_MODULE(Operator, "Operator", "Fm")
DEFINE_MODULE(Envelope, "Envelope", "Fm")

void InitAugrFmLibrary() {
    REGISTER_MODULE(Operator);
    REGISTER_MODULE(Envelope);
}
