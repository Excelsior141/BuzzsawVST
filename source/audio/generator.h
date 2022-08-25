#pragma once

#include <cstdlib>

#include "../utility/logger.h"
#include "../math/const.h"

#include "pitch.h"
#include "envelope.h"

namespace exco
{
	namespace generator
	{
		/* TODO: Think of a way to unify checks */

		// Random values from -1.0f to 1.0f
		void noise(float* output, int length)
		{
			if (!output)    { Logger::Write("No output");      return; }
			if (length < 0) { Logger::Write("Invalid length"); return; }

			for (unsigned int i = 0; i < length; ++i)
				output[i] = (static_cast<float>(2 * std::rand()) / RAND_MAX) - 1.0f;
		}

		void sine(unsigned long long state, float* output, int length, unsigned int samplesPerSecond, float frequency)
		{
			if (!output)               { Logger::Write("No output");                  return; }
			if (length < 0)            { Logger::Write("Invalid length");             return; }
			if (samplesPerSecond == 0) { Logger::Write("Invalid samples per second"); return; }
			if (frequency < 0)         { Logger::Write("Invalid frequency");          return; }

			float cyclesPerSample = frequency / samplesPerSecond;
			float angleDelta = cyclesPerSample * math::k2PId;
			float currentAngle = 0.0f;

			for (unsigned int i = 0; i < length; ++i)
			{
				output[i] = std::sinf(currentAngle);
				currentAngle += angleDelta;
			}
		}
	} // namespace generator
} // namespace exco