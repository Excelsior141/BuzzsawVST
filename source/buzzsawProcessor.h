#pragma once

#include "public.sdk/source/vst/vstaudioeffect.h"
#include "pluginterfaces/vst/ivstevents.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"

#include "subprocessors/engineSubprocessor.h"

#include <string>

namespace exco {

using namespace Steinberg;
using namespace Vst;

struct Note
{
	short pitch = -1;
	float frequency = -1.0f;
	float velocity = -1.0f;
	int sampleOffset = -1;
};

class BuzzsawProcessor : public AudioEffect
{
public:

	BuzzsawProcessor();
	~BuzzsawProcessor();
	
	tresult PLUGIN_API initialize (FUnknown* context) SMTG_OVERRIDE;
	tresult PLUGIN_API setActive (TBool state) SMTG_OVERRIDE;

	tresult PLUGIN_API process(ProcessData& data) SMTG_OVERRIDE;

	tresult PLUGIN_API setBusArrangements(SpeakerArrangement* inputs, int32 numIns,
										  SpeakerArrangement* outputs, int32 numOuts) SMTG_OVERRIDE;

	static FUnknown* createInstance (void*) { return (IAudioProcessor*)new BuzzsawProcessor; }
	static FUID uid;

private:

	Note lastNote {};

	static const unsigned int kFrameNoteBufferSize = 256;
	Note frameNoteBuffer[kFrameNoteBufferSize] {};
	unsigned int lastFrameNoteBufferEntryIndex = 0;

private:

	EngineSubprocessor engineSub;
};

} // namespace exco
