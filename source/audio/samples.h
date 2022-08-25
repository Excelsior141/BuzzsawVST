#pragma once

#include "../utility/logger.h"
#include "../math/const.h"

#include "pitch.h"
#include "envelope.h"
#include "generator.h"

namespace exco
{
	namespace samples
	{
		using Sample = std::vector<float>;

		const Sample combustion = []() -> Sample
		{
			const unsigned int sampleSize = 44100;
			const unsigned int samplePercent = sampleSize / 100;

			const double frequency = 90.0;

			auto getAngleDelta = [&sampleSize](double frequency) -> double {
				double cyclesPerSample = frequency / sampleSize;
				return cyclesPerSample * math::k2PId;
			};

			Sample subKick;
			{
				subKick.resize(sampleSize);
				double currentAngle = 0.0;

				for (int i = 0; i < subKick.size(); ++i)
				{
					double graphX = static_cast<double>(i) / subKick.size();
					double frequencyMultiplier = -(graphX * graphX) + 1.0;

					subKick[i] = std::sin(currentAngle);
					currentAngle += getAngleDelta(frequency * frequencyMultiplier);
				}

				envelope::applyLinear(subKick.data()                    , samplePercent                     , 0.0f , 1.0f);
				envelope::applyLinear(subKick.data() + samplePercent * 2, samplePercent * 2                 , 1.0f , 0.05f);
				envelope::applyLinear(subKick.data() + samplePercent * 4, subKick.size() - samplePercent * 4, 0.05f, 0.0f);
			}

			Sample noise;
			{
				noise.resize(sampleSize);
				generator::noise(noise.data(), noise.size());

				envelope::applyLinear(noise.data()                    , samplePercent                     , 0.0f , 1.0f);
				envelope::applyLinear(noise.data() + samplePercent    , samplePercent * 3                 , 1.0f , 0.05f);
				envelope::applyLinear(noise.data() + samplePercent * 4, subKick.size() - samplePercent * 4, 0.05f, 0.0f);
			}

			Sample mix;
			{
				mix.resize(sampleSize);

				for (int i = 0; i < mix.size(); ++i)
				{
					mix[i] = 0.8f * noise[i] +
							 0.2f * subKick[i];
				}
			}

			envelope::normalize(mix.data(), mix.size());

			envelope::blurBidirectionalCumulative(mix.data(), mix.size(), 110);

			envelope::applyLinear(mix.data(), samplePercent, 0.0f, 1.0f);
			envelope::applyLinear(mix.data() + samplePercent * 99, samplePercent, 1.0f, 0.0f);

			envelope::normalize(mix.data(), mix.size());

			return mix;
		}();

		const Sample valveOpen = []() -> Sample
		{
			const unsigned int sampleSize = 44100;
			const unsigned int samplePercent = sampleSize / 100;

			const double frequency = 60.0;

			auto getAngleDelta = [&sampleSize](double frequency) -> double {
				double cyclesPerSample = frequency / sampleSize;
				return cyclesPerSample * math::k2PId;
			};

			Sample noise;
			{
				noise.resize(sampleSize);
				generator::noise(noise.data(), noise.size());

				envelope::applyLinear(noise.data()                     , samplePercent                    , 0.0f , 1.0f);
				envelope::applyLinear(noise.data() + samplePercent     , samplePercent * 32               , 1.0f , 0.33f);
				envelope::applyLinear(noise.data() + samplePercent * 33, noise.size() - samplePercent * 33, 0.33f, 0.0f);
			}

			Sample masterEnvelope;
			{
				masterEnvelope.resize(sampleSize);
				double currentAngle = 0.0;

				for (int i = 0; i < masterEnvelope.size(); ++i)
				{
					double graphX = static_cast<double>(i) / masterEnvelope.size();
					double frequencyMultiplier = fmin(graphX * graphX + 0.5, 1.0);

					masterEnvelope[i] = std::sin(currentAngle);
					currentAngle += getAngleDelta(frequency * frequencyMultiplier);
				}

				envelope::applyLinear(masterEnvelope.data(), samplePercent, 0.0f, 1.0f);
				envelope::applyLinear(masterEnvelope.data() + samplePercent, masterEnvelope.size() - samplePercent, 1.0f, 0.0f);

				envelope::positivise(masterEnvelope.data(), masterEnvelope.size());
				envelope::normalize(masterEnvelope.data(), masterEnvelope.size());
			}

			Sample mix;
			{
				mix.resize(sampleSize);

				for (int i = 0; i < mix.size(); ++i)
					mix[i] = noise[i] * ((masterEnvelope[i] + 0.75f) * (1.0f / 1.75f));
			}

			envelope::blurBidirectional(mix.data(), mix.size(), 1);

			envelope::applyLinear(mix.data(), samplePercent, 0.0f, 1.0f);
			envelope::applyLinear(mix.data() + samplePercent * 99, samplePercent, 1.0f, 0.0f);

			envelope::normalize(mix.data(), mix.size());

			return mix;
		}();

		// Just a mirrored version of valveOpen
		const Sample valveClose = []() -> Sample 
		{
			Sample result;
			result.resize(valveOpen.size());

			for (int i = 0; i < result.size(); ++i)
				result[i] = valveOpen[valveOpen.size() - (i + 1)];

			return result;
		}();
	} // namespace samples
} // namespace exco