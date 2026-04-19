#include <augr/core/model_manufacturer.h>
#include <augr/core/model.h>

#include <augr/volt/vco_module.h>
#include <augr/volt/control_voltage.h>

using namespace augr;

DEFINE_MODEL_FACTORY(VcoModule, "VcoModule", "Volt")
DEFINE_MODEL_FACTORY(ControlVoltage, "ControlVoltage", "Volt")

void InitAugrVoltLibrary() {
    REGISTER_MODEL_FACTORY(VcoModule);
    REGISTER_MODEL_FACTORY(ControlVoltage);
}
