#pragma once

#include "public.sdk/source/vst/vstparameters.h"

namespace Steinberg {
namespace Panner {

// HERE are defined the parameter Ids which are exported to the host
enum PannerParams : Vst::ParamID
{
	kBypassId = 100,

	kParamPanId = 102,
};


// HERE you have to define new unique class ids: for processor and for controller
// you can use GUID creator tools like https://www.guidgenerator.com/
static const FUID MyProcessorUID (0xf0e56758, 0x7be84792, 0xb36569fe, 0x63fd202b);
static const FUID MyControllerUID (0x74c39f88, 0x5d984426, 0xae4d7ec0, 0xa69972ce);

//------------------------------------------------------------------------
} // namespace Panner
} // namespace Steinberg
