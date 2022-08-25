#include "engineSubprocessor.h"

#include "../buzzsawProcessor.h"

namespace exco
{
	EngineSubprocessor::EngineSubprocessor()
	{

	}

	EngineSubprocessor::~EngineSubprocessor()
	{

	}

	bool EngineSubprocessor::process(float* leftOutput, float* rightOutput, int frameSamples, double sampleRate, const Note* frameNoteBuffer, unsigned int frameNoteCount)
	{
		for (unsigned int noteBufferEntryIndex = 0; noteBufferEntryIndex < frameNoteCount; ++noteBufferEntryIndex)
		{
			const Note& currentNote = frameNoteBuffer[noteBufferEntryIndex];
			if (currentNote.pitch == -1) continue;

			const Note const* nextNote = noteBufferEntryIndex > frameNoteCount ? nullptr
				: &frameNoteBuffer[noteBufferEntryIndex + 1];

			int length = (nextNote == nullptr ? frameSamples : nextNote->sampleOffset) - currentNote.sampleOffset;

		}

		return true;
	}

	void EngineSubprocessor::resetState()
	{

	}
} // namespace exco