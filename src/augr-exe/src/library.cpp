#include <augr/core/model_manufacturer.h>
#include <augr/core/archiver_manufacturer.h>

#include <augr/exe/rack/exe_rack.h>

#include <augr/rack/archiver/module_archiver.h>
#include <augr/rack/archiver/rack_archiver.h>

using namespace augr;


void InitAugrExeLibrary() {
    REGISTER_MODEL_FACTORY(ExeRack);
    REGISTER_ARCHIVER_FACTORY(ExeRackArchiver);
}
