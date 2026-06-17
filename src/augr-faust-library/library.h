#pragma once

#include <augr/model_manufacturer.h>
//#include <augr/archiver_factory.h>
#include <augr/archiver_manufacturer.h>

#include <augr/rack/archiver/module_archiver.h>

#include <augr/faust/faust_dsp.h>
#include <augr/faust/faust_dsp_ui.h>


void InitFaustDspLibrary();
void InitFaustDspLibrary_Analysis();
void InitFaustDspLibrary_GameAudio();
void InitFaustDspLibrary_Generator();
void InitFaustDspLibrary_Phasing();
void InitFaustDspLibrary_PhysicalModeling();
void InitFaustDspLibrary_PhysicalModeling_FaustStk();
