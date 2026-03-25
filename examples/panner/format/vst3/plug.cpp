#include "public.sdk/source/main/pluginfactory.h"

#include "plug_editor_impl.h"	// for createInstance
#include "plug.h"	// for createInstance
#include "parameters.h"			// for uids
#include "version.h"			// for version and naming

#define stringSubCategory Vst::PlugType::kSpatialFx	// Subcategory for this plug-in (to be changed if needed, see PlugType in ivstaudioprocessor.h)

BEGIN_FACTORY_DEF (stringCompanyName, stringCompanyWeb,	stringCompanyEmail)

	DEF_CLASS2 (INLINE_UID_FROM_FUID(Steinberg::Panner::MyProcessorUID),
				PClassInfo::kManyInstances,	// cardinality  
				kVstAudioEffectClass,	// the component category (do not changed this)
				stringPluginName,		// here the plug-in name (to be changed)
				Vst::kDistributable,	// means that component and controller could be distributed on different computers
				stringSubCategory,		// Subcategory for this plug-in (to be changed)
				FULL_VERSION_STR,		// Plug-in version (to be changed)
				kVstVersionString,		// the VST 3 SDK version (do not changed this, use always this define)
				Steinberg::Panner::Plug::createInstance)	// function pointer called when this component should be instantiated

	DEF_CLASS2 (INLINE_UID_FROM_FUID(Steinberg::Panner::MyControllerUID),
				PClassInfo::kManyInstances,  // cardinality   
				kVstComponentControllerClass,// the Controller category (do not changed this)
				stringPluginName "Controller",	// controller name (could be the same than component name)
				0,						// not used here
				"",						// not used here
				FULL_VERSION_STR,		// Plug-in version (to be changed)
				kVstVersionString,		// the VST 3 SDK version (do not changed this, use always this define)
				Steinberg::Panner::PannerEditorImpl::createInstance)// function pointer called when this component should be instantiated

END_FACTORY

