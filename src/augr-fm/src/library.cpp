#include <augr/core/model_manufacturer.h>

#include <augr/fm/operator.h>


using namespace augr;
using namespace augr::fm;

DEFINE_MODEL_FACTORY(Operator, "Operator", "Fm")

void InitAugrFmLibrary() {
    REGISTER_MODEL_FACTORY(Operator);
}
