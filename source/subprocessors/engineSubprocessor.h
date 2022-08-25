#pragma once

namespace exco
{
	struct Note;

	enum class CylinderEventType
	{
		Combustion,
		ValveOpen,
		ValveClose,
		Idle,
	};

	const std::vector<CylinderEventType> simpleFourStroke
	{
		ValveOpen,
		ValveClose,
		Combustion,
		ValveOpen,
		ValveClose,
	};

	class EngineSubprocessor
	{
	public:

		EngineSubprocessor();
		~EngineSubprocessor();

		bool process(float* leftOutput, float* rightOutput, int frameSamples, double sampleRate, const Note* frameNoteBuffer, unsigned int frameNoteCount);
		void resetState();

	private:

		float rotationsPerSecond = 0.0f;
		float totalTime = 0.0f;
	};

} // namespace exco