#include "public.sdk/source/main/pluginfactory.h"

#include "buzzsawController.h"

//-----------------------------------------------------------------------------
bool InitModule () { return true; }
bool DeinitModule () { return true; }

//-----------------------------------------------------------------------------
// TODO: Probably still want to have these defines in a single file 
#define kVersionString	"0.0.1"

using namespace Steinberg;
using namespace Vst;

BEGIN_FACTORY_DEF("Exco", 
				  "http://www.exco.lt", 
				  "mailto:vils14@windowslive.com")

DEF_CLASS2(INLINE_UID_FROM_FUID (exco::BuzzsawProcessor::uid),
		   PClassInfo::kManyInstances,
		   kVstAudioEffectClass,
		   "Buzzsaw",
		   Vst::kDistributable,
		   Vst::PlugType::kInstrumentSynthSampler,
		   kVersionString,
		   kVstVersionString,
		   exco::BuzzsawProcessor::createInstance)

DEF_CLASS2(INLINE_UID_FROM_FUID (exco::BuzzsawController::uid),
		   PClassInfo::kManyInstances,
		   kVstComponentControllerClass,
		   "Buzzsaw",
		   Vst::kDistributable,
		   "",
		   kVersionString,
		   kVstVersionString,
		   exco::BuzzsawController::createInstance)

END_FACTORY

