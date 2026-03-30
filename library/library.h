#pragma once

#include <augr/core/rack/model_manufacturer.h>
#include <augr/core/rack/module/faust_dsp.h>
#include <augr/core/rack/module/faust_dsp_ui.h>
#include <augr/core/rack/module/module.h>


void InitFaustDspLibrary();
void InitFaustDspLibrary_Analysis();
void InitFaustDspLibrary_GameAudio();
void InitFaustDspLibrary_Phasing();
void InitFaustDspLibrary_PhysicalModeling();
void InitFaustDspLibrary_PhysicalModeling_FaustStk();
