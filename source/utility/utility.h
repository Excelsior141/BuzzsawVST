#pragma once

#include <stdio.h>
#include "logger.h"

static char* ReadFile(const char* pFilename, unsigned int& rFileBufferSize)
{
	char* pFileBuffer = nullptr;
	FILE* pFileHandle = nullptr;
	fopen_s(&pFileHandle, pFilename, "rb");

	if (!pFileHandle)
		return nullptr;

	fseek(pFileHandle, 0, SEEK_END);
	rFileBufferSize = ftell(pFileHandle);
	fseek(pFileHandle, 0, SEEK_SET);

	pFileBuffer = new char[rFileBufferSize];

	fread(pFileBuffer, rFileBufferSize, sizeof(char), pFileHandle);
	fclose(pFileHandle);

	return pFileBuffer;
}

static void WriteFile(char* pFilebuffer, unsigned int fileBufferSize, const char* pFilename, const char* pMode)
{
	FILE* pFileHandle = nullptr;
	fopen_s(&pFileHandle, pFilename, "wb");

	fwrite(pFilebuffer, sizeof(char), fileBufferSize, pFileHandle);

	fclose(pFileHandle);
}

using namespace Steinberg;
using namespace Vst;
using namespace exco;

const char* getNoteExpressionTypeString(NoteExpressionTypeID typeId)
{
	switch (typeId)
	{
		case NoteExpressionTypeIDs::kVolumeTypeID:	   return "Volume";
		case NoteExpressionTypeIDs::kPanTypeID:        return "Pan";
		case NoteExpressionTypeIDs::kTuningTypeID:     return "Tuning";
		case NoteExpressionTypeIDs::kVibratoTypeID:    return "Vibrato";
		case NoteExpressionTypeIDs::kExpressionTypeID: return "Expression";
		case NoteExpressionTypeIDs::kBrightnessTypeID: return "Brightness";
		case NoteExpressionTypeIDs::kTextTypeID:       return "Text";
		case NoteExpressionTypeIDs::kPhonemeTypeID:    return "Phoneme";
		case NoteExpressionTypeIDs::kInvalidTypeID:    return "Invalid";
		default:
		{
			if (typeId >= NoteExpressionTypeIDs::kCustomStart && typeId < NoteExpressionTypeIDs::kCustomEnd)
				return "Custom";
			else if (typeId == NoteExpressionTypeIDs::kCustomEnd)
				return "Ambiguous: custom or unknown - event type ID is equal to kCustomEnd";
			else if (typeId > NoteExpressionTypeIDs::kCustomEnd)
				return "Unknown (> kCustomEnd)";
		} break;
	};

	return "Completely screwed up";
}

const char* getEventTypeString(uint16 eventType)
{
	switch (eventType)
	{
		case Event::kNoteOnEvent:			   return "Note On";
		case Event::kNoteOffEvent:			   return "Note Off";
		case Event::kDataEvent:				   return "Data";
		case Event::kPolyPressureEvent:		   return "Poly pressure";
		case Event::kNoteExpressionValueEvent: return "Note expression value";
		case Event::kNoteExpressionTextEvent:  return "Note expression text";
		case Event::kChordEvent:			   return "Chord";
		case Event::kScaleEvent:			   return "Scale";
		case Event::kLegacyMIDICCOutEvent:	   return "Legacy MIDI CC Out";
	};

	return "Completely screwed up";
}

template <typename T>
static void getBitmaskString(T mask, std::string& outStr)
{
	constexpr std::size_t sizeT = sizeof(T) * 8;
	outStr.resize(sizeT);

	for (int i = 0; i < sizeT; ++i)
		outStr[i] = ((mask >> i) & 1 ? '1' : '0');
}

static void debugVstEvent(Event& event)
{
	std::string strMask;
	getBitmaskString(event.flags, strMask);

	Logger::Write("%s event: ", getEventTypeString(event.type));
	Logger::Write("    Bus Index:                           %d", event.busIndex);
	Logger::Write("    Sample offset:                       %d", event.sampleOffset);
	Logger::Write("    Position in project (quarter notes): %f", event.ppqPosition);
	Logger::Write("    Flags:                               %s", strMask.c_str());

	switch (event.type)
	{
		case Event::kNoteOnEvent:
		{
			const NoteOnEvent& noteOnEvent = event.noteOn;

			Logger::Write("    Channel:  %hi", noteOnEvent.channel); ///< channel index in event bus
			Logger::Write("    Length:   %d", noteOnEvent.length);	 ///< in sample frames (optional, Note Off has to follow in any case!)
			Logger::Write("    Note ID:  %d", noteOnEvent.noteId);	 ///< note identifier (if not available then -1)
			Logger::Write("    Pitch:    %hi", noteOnEvent.pitch);   ///< range [0, 127] = [C-2, G8] with A3=440Hz (12-TET)
			Logger::Write("    Tuning:   %f", noteOnEvent.tuning);	 ///< 1.f = +1 cent, -1.f = -1 cent
			Logger::Write("    Velocity: %f (%hi)", noteOnEvent.velocity, int16(noteOnEvent.velocity * 127)); ///< range [0.0, 1.0]

		} break;
		case Event::kNoteOffEvent:
		{
			const NoteOffEvent& noteOffEvent = event.noteOff;

			Logger::Write("    Channel:  %hi", noteOffEvent.channel); ///< channel index in event bus
			Logger::Write("    Note ID:  %d", noteOffEvent.noteId);   ///< associated noteOn identifier (if not available then -1)
			Logger::Write("    Pitch:    %hi", noteOffEvent.pitch);   ///< range [0, 127] = [C-2, G8] with A3=440Hz (12-TET)
			Logger::Write("    Tuning:   %f", noteOffEvent.tuning);   ///< 1.f = +1 cent, -1.f = -1 cent
			Logger::Write("    Velocity: %f (%hi)", noteOffEvent.velocity, int16(noteOffEvent.velocity * 127)); ///< range [0.0, 1.0]

		} break;
		case Event::kDataEvent:
		{
			const DataEvent& dataEvent = event.data;

			Logger::Write("    Bytes?: %s", (dataEvent.bytes != nullptr ? "true" : "false"));
			Logger::Write("    Size:   %i", dataEvent.size);
			Logger::Write("    Type:   kMidiSysEx (confirmed? %s)", (dataEvent.type == DataEvent::DataTypes::kMidiSysEx ? "true" : "false"));
		} break;
		case Event::kPolyPressureEvent:
		{
			const PolyPressureEvent& polyPressureEvent = event.polyPressure;

			Logger::Write("    Channel:  %hi", polyPressureEvent.channel);
			Logger::Write("    Note ID:  %d", polyPressureEvent.noteId);
			Logger::Write("    Pitch :   %hi", polyPressureEvent.pitch);
			Logger::Write("    Pressure: %f", polyPressureEvent.pressure);

		} break;
		case Event::kNoteExpressionValueEvent:
		{
			const NoteExpressionValueEvent& noteExpressionValueEvent = event.noteExpressionValue;

			Logger::Write("    Note ID: %d", noteExpressionValueEvent.noteId);
			Logger::Write("    Type:    %s", noteExpressionValueEvent.typeId);
			Logger::Write("    Value:   %lf", noteExpressionValueEvent.value);

		} break;
		case Event::kNoteExpressionTextEvent:
		{
			const NoteExpressionTextEvent& noteExpressionTextEvent = event.noteExpressionText;

			std::wstring wStr = noteExpressionTextEvent.text;
			std::string str(wStr.begin(), wStr.end());

			Logger::Write("    Note ID:     %d", noteExpressionTextEvent.noteId);
			Logger::Write("    Type:        %s", getNoteExpressionTypeString(noteExpressionTextEvent.typeId));
			Logger::Write("    Text:        %s", str.c_str());
			Logger::Write("    Text length: %i", noteExpressionTextEvent.textLen);

		} break;
		case Event::kChordEvent:
		{
			const ChordEvent& chordEvent = event.chord;

			std::wstring wStr = chordEvent.text;
			std::string str(wStr.begin(), wStr.end());
			getBitmaskString(chordEvent.mask, strMask);

			Logger::Write("    Bass note:   %hi", chordEvent.bassNote);
			Logger::Write("    Root:        %hi", chordEvent.root);
			Logger::Write("    Mask:        %s", strMask.c_str());
			Logger::Write("    Text:        %s", str.c_str());
			Logger::Write("    Text length: %i", chordEvent.textLen);

		} break;
		case Event::kScaleEvent:
		{
			const ScaleEvent& scaleEvent = event.scale;

			std::wstring wStr = scaleEvent.text;
			std::string str(wStr.begin(), wStr.end());
			getBitmaskString(scaleEvent.mask, strMask);

			Logger::Write("    Root:        %hi", scaleEvent.root);
			Logger::Write("    Mask:        %s", strMask.c_str());
			Logger::Write("    Text:        %s", str.c_str());
			Logger::Write("    Text length: %i", scaleEvent.textLen);

		} break;
		case Event::kLegacyMIDICCOutEvent:
		{
			const LegacyMIDICCOutEvent& legacyMIDICCOutEvent = event.midiCCOut;

			Logger::Write("    Channel:        %hi", legacyMIDICCOutEvent.channel);
			Logger::Write("    Control number: %hu", legacyMIDICCOutEvent.controlNumber);
			Logger::Write("    Value 1:        %hi", legacyMIDICCOutEvent.value);
			Logger::Write("    Value 2:        %hi", legacyMIDICCOutEvent.value2);

		} break;
	}
}
