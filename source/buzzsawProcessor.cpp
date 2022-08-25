#include "buzzsawProcessor.h"
#include "buzzsawController.h"

#include "utility/logger.h"
#include "utility/utility.h"

#include "audio/pitch.h"
#include "audio/generator.h"
#include "audio/envelope.h"
#include "audio/samples.h"

namespace exco {

using namespace Steinberg;
using namespace Vst;

FUID BuzzsawProcessor::uid(0x38DFF405, 0xC54A1B55, 0x09A97E9D, 0x171631B9);

BuzzsawProcessor::BuzzsawProcessor()
{
	Logger::Write("BuzzsawProcessor()");
	setControllerClass(BuzzsawController::uid);
}

BuzzsawProcessor::~BuzzsawProcessor()
{
	Logger::Write("~BuzzsawProcessor()");
}

tresult PLUGIN_API BuzzsawProcessor::initialize(FUnknown* context)
{
	tresult res = AudioEffect::initialize (context);
	if (res == kResultTrue)
	{
		addEventInput(USTRING("MIDI in"), 1);
		addAudioOutput(USTRING("Stereo Out"), SpeakerArr::kStereo);
	}

	return res;
}

tresult PLUGIN_API BuzzsawProcessor::setActive(TBool state)
{
	if (!state)
	{
		lastFrameNoteBufferEntryIndex = 0;
		engineSub.resetState();
	}
		
	return AudioEffect::setActive(state);
}

tresult PLUGIN_API BuzzsawProcessor::process(ProcessData& data)
{
	lastFrameNoteBufferEntryIndex = 0;
	frameNoteBuffer[lastFrameNoteBufferEntryIndex] = lastNote;

	if (data.inputEvents)
	{
		IEventList* events = data.inputEvents;
		int32 eventCount = events->getEventCount();

		// Parse events, update noteBuffer and noteBufferHeadRecords
		for (int32 eventIndex = 0; eventIndex < eventCount; ++eventIndex)
		{
			Event event;
			if (events->getEvent(eventIndex, event) == kResultFalse)
				continue;

			if (lastFrameNoteBufferEntryIndex + 1 >= kFrameNoteBufferSize)
				break;

			switch (event.type)
			{
			case Event::kNoteOnEvent:
			{
				const NoteOnEvent& noteOnEvent = event.noteOn;

				// Overwrite at last index otherwise
				if (frameNoteBuffer[lastFrameNoteBufferEntryIndex].sampleOffset <= event.sampleOffset)
					++lastFrameNoteBufferEntryIndex;

				float baseFrequency = pitch::kFrequencyTable[noteOnEvent.pitch];
				float tuneFrequency = noteOnEvent.tuning > 0.0f ? (noteOnEvent.pitch + 1 <= 127 ? pitch::kFrequencyTable[noteOnEvent.pitch + 1] : pitch::kMaxFrequency)
					                                            : (noteOnEvent.pitch - 1 >= 0   ? pitch::kFrequencyTable[noteOnEvent.pitch - 1] : pitch::kMinFrequency);

				Note& note = frameNoteBuffer[lastFrameNoteBufferEntryIndex];

				note.pitch = noteOnEvent.pitch;
				note.frequency = baseFrequency + (((tuneFrequency - baseFrequency) / 100) * noteOnEvent.tuning);
				note.velocity = noteOnEvent.velocity;
				note.sampleOffset = event.sampleOffset;
			} break;
			case Event::kNoteOffEvent:
			{
				const NoteOffEvent& noteOffEvent = event.noteOff;

				if (noteOffEvent.pitch == frameNoteBuffer[lastFrameNoteBufferEntryIndex].pitch)
				{
					Note& note = frameNoteBuffer[++lastFrameNoteBufferEntryIndex];

					note.pitch = -1;
					note.frequency = -1.0f;
					note.velocity = -1.0f;
					note.sampleOffset = event.sampleOffset;
				}
			} break;
			default:
			{
				Logger::Write("BuzzsawProcessor::process - Unhandled event %s received", getEventTypeString(event.type));
				debugVstEvent(event);
			} break;
			};
		}
	}

	if (data.outputs)
	{
		int32 frameSamples = data.numSamples;
		double sampleRate = processSetup.sampleRate;

		// In actuality the channel sides could be anything, keeping to this convention though
		// nullptr if silent
		Sample32* leftOutput  =  data.outputs[0].silenceFlags & 1       ? nullptr : data.outputs[0].channelBuffers32[0];
		Sample32* rightOutput = (data.outputs[0].silenceFlags >> 1) & 1 ? nullptr : data.outputs[0].channelBuffers32[1];

		// Be mindful about the symbolic sample size (it has to be 32-bit to fit with ints that are setting it 0 currently)
		if (leftOutput  == nullptr) memset(data.outputs[0].channelBuffers32[0], 0, frameSamples * sizeof(float));
		if (rightOutput == nullptr) memset(data.outputs[0].channelBuffers32[1], 0, frameSamples * sizeof(float));

		// TODO: Calculate a weight (1.0 / subprocessor count (obv >= 1) * subprocessor volume [0.0;1.0])
		engineSub.process(leftOutput, rightOutput, frameSamples, sampleRate, frameNoteBuffer, lastFrameNoteBufferEntryIndex + 1);
	}

	lastNote = frameNoteBuffer[lastFrameNoteBufferEntryIndex];
	lastNote.sampleOffset = 0;

	return kResultTrue;
}

tresult PLUGIN_API BuzzsawProcessor::setBusArrangements(SpeakerArrangement* inputs, int32 numIns, SpeakerArrangement* outputs, int32 numOuts)
{
	return SpeakerArr::getChannelCount(outputs[0]) == 2;
}

} // namespace exco

