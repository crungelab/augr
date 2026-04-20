#include <augr/core/model_manufacturer.h>
#include <augr/core/model.h>

#include <augr/volt/cv_module.h>
#include <augr/volt/vco_module.h>
#include <augr/volt/vca_module.h>

using namespace augr;

DEFINE_MODEL_FACTORY(CvModule, "CvModule", "Volt")
DEFINE_MODEL_FACTORY(VcoModule, "VcoModule", "Volt")
DEFINE_MODEL_FACTORY(VcaModule, "VcaModule", "Volt")

void InitAugrVoltLibrary() {
    REGISTER_MODEL_FACTORY(CvModule);
    REGISTER_MODEL_FACTORY(VcoModule);
    REGISTER_MODEL_FACTORY(VcaModule);
}
