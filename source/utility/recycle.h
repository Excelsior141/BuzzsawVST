#pragma once

// Just a bay for commented out code which might be useful in the future
/*

// Used to slap attack and release envelopes on notes that are just starting or are predicted to end (if note ends along with the frame, cannot apply release)
{
	const unsigned int envStateLength = static_cast<unsigned int>(std::floor((sampleRate / 1000) * 16));

	bool attack = currentNote.trackedLength < envStateLength;
	bool release = (nextNote != nullptr);

	if (attack)
	{
		unsigned int attackSamplesLeft = attackSamplesLeft > length ? length
																	: envStateLength - currentNote.trackedLength;

		float from = float(currentNote.trackedLength)          / float(envStateLength); // beware dbz
		float to   = float(currentNote.trackedLength + length) / float(envStateLength); // beware dbz

		if (to > 1.0f) to = 1.0f;

		if (leftOutput  != nullptr) envelope::applyLinear(leftOutput  + currentNote.sampleOffset, attackSamplesLeft, from, to);
		if (rightOutput != nullptr) envelope::applyLinear(rightOutput + currentNote.sampleOffset, attackSamplesLeft, from, to);
	}

	if (release)
	{
		unsigned int releaseLength = envStateLength > length ? length : envStateLength;
		unsigned int releaseStartSampleOffset = (currentNote.sampleOffset + length) - releaseLength;

		if (leftOutput  != nullptr) envelope::applyLinear(leftOutput  + releaseStartSampleOffset, releaseLength, 1.0f, 0.0f);
		if (rightOutput != nullptr) envelope::applyLinear(rightOutput + releaseStartSampleOffset, releaseLength, 1.0f, 0.0f);
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// Implementation of BuzzsawProcessor::process for single sample playthrough (testing)

// class fields:
		Note lastNote {};
		
		static const unsigned int kFrameNoteBufferSize = 256;
		Note frameNoteBuffer[kFrameNoteBufferSize] {};
		unsigned int lastFrameNoteBufferEntryIndex = 0;

// also struct Note had an additional field unsigned int trackedLength = 0;

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
				note.trackedLength = 0;
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
					note.trackedLength = 0;
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

		for (unsigned int noteBufferEntryIndex = 0; noteBufferEntryIndex < lastFrameNoteBufferEntryIndex + 1; ++noteBufferEntryIndex)
		{
			const Note& currentNote = frameNoteBuffer[noteBufferEntryIndex];
			if (currentNote.pitch == -1) continue;

			const Note const* nextNote = noteBufferEntryIndex + 1 > lastFrameNoteBufferEntryIndex ? nullptr
																								  : &frameNoteBuffer[noteBufferEntryIndex + 1];

			int length = (nextNote == nullptr ? frameSamples : nextNote->sampleOffset) - currentNote.sampleOffset;
			int sampleLengthLeft = samples::hihat.size() - currentNote.trackedLength;

			int samplesToSilenceBefore = currentNote.sampleOffset == 0      ? 0                      : length - currentNote.sampleOffset;
			int samplesToPlay          = sampleLengthLeft          > length ? length                 : sampleLengthLeft;
			int samplesToSilenceAfter  = samplesToPlay             > 0      ? length - samplesToPlay : length;

			samplesToPlay              = samplesToPlay < 0 ? 0 : samplesToPlay;

			int bytesToSilenceBefore = samplesToSilenceBefore * sizeof(float);
			int bytesToPlay          = samplesToPlay          * sizeof(float);
			int bytesToSilenceAfter  = samplesToSilenceAfter  * sizeof(float);

			if (leftOutput != nullptr)
			{
				if (samplesToSilenceBefore > 0) memset(leftOutput, 0, bytesToSilenceBefore);
				if (samplesToPlay          > 0) memcpy(leftOutput + currentNote.sampleOffset, samples::hihat.data() + currentNote.trackedLength, bytesToPlay);
				if (samplesToSilenceAfter  > 0) memset(leftOutput + currentNote.sampleOffset + samplesToPlay, 0, bytesToSilenceAfter);
			}

			if (rightOutput != nullptr)
			{
				if (samplesToSilenceBefore > 0) memset(rightOutput, 0, bytesToSilenceBefore);
				if (samplesToPlay          > 0) memcpy(rightOutput + currentNote.sampleOffset, samples::hihat.data() + currentNote.trackedLength, bytesToPlay);
				if (samplesToSilenceAfter  > 0) memset(rightOutput + currentNote.sampleOffset + samplesToPlay, 0, bytesToSilenceAfter);
			}

			frameNoteBuffer[noteBufferEntryIndex].trackedLength += samplesToPlay;
		}

		// TODO: Might not be able to do this for multiple subprocessors - need a mixer or pass a weight (1/subprocessor count)
		//engineSub.process(leftOutput, rightOutput, frameSamples, sampleRate, frameNoteBuffer, lastFrameNoteBufferEntryIndex);
	}

	lastNote = frameNoteBuffer[lastFrameNoteBufferEntryIndex];
	lastNote.sampleOffset = 0;

	return kResultTrue;
}
*/