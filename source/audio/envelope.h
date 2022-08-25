#pragma once

namespace exco
{
	namespace envelope
	{
		void linear(float* output, int length, float from, float to)
		{
			if (output == nullptr || length <= 0) return;

			float step = (to - from) / length;
			for (int i = 0; i < length; ++i)
				output[i] = from + (step * i);
		}

		void applyLinear(float* output, int length, float from, float to)
		{
			if (output == nullptr || length <= 0) return;

			float step = (to - from) / length;
			for (int i = 0; i < length; ++i)
				output[i] *= from + (step * i);
		}

		// Move to effect.h?
		void normalize(float* output, int length)
		{
			if (output == nullptr || length <= 0) return;

			float min = std::numeric_limits<float>::max();
			float max = std::numeric_limits<float>::min();
			for (int i = 0; i < length; ++i)
			{
				min = fmin(min, output[i]);
				max = fmax(max, output[i]);
			}

			max = fmax(fabs(min), fabs(max));

			if (max == 0.0f) return;

			for (int i = 0; i < length; ++i)
				output[i] /= max;
		}

		void positivise(float* output, int length)
		{
			if (output == nullptr || length <= 0) return;

			float min = std::numeric_limits<float>::max();
			for (int i = 0; i < length; ++i)
				min = fmin(min, output[i]);

			if (min >= 0.0f) return;

			for (int i = 0; i < length; ++i)
				output[i] += fabs(min);
		}

		float getSampleSafe(float* output, int length, int index)
		{
			if (index < 0 || index >= length) return 0.0f;
			return output[index];
		}

		void blurBidirectional(float* output, int length, int strength)
		{
			if (output == nullptr || length <= 0 || strength <= 0) return;

			std::vector<float> pristine;
			pristine.resize(length);
			memcpy(pristine.data(), output, length * sizeof(float));

			for (int i = 0; i < length; ++i)
			{
				float sum = 0.0f;
				for (int x = -strength; x <= strength; ++x)
					sum += getSampleSafe(pristine.data(), length, i + x);

				output[i] = sum / (strength * 2 + 1);
			}
		}

		void blurBidirectionalCumulative(float* output, int length, int strength)
		{
			if (output == nullptr || length <= 0 || strength <= 0) return;

			for (int i = 0; i < length; ++i)
			{
				float sum = 0.0f;
				for (int x = -strength; x <= strength; ++x)
					sum += getSampleSafe(output, length, i + x);

				output[i] = sum / (strength * 2 + 1);
			}
		}

		void blurBackward(float* output, int length, int strength)
		{
			if (output == nullptr || length <= 0 || strength <= 0) return;

			std::vector<float> pristine;
			pristine.resize(length);
			memcpy(pristine.data(), output, length * sizeof(float));

			for (int i = 0; i < length; ++i)
			{
				float sum = 0.0f;
				for (int x = 0; x <= strength; ++x)
					sum += getSampleSafe(pristine.data(), length, i + x);

				output[i] = sum / (strength + 1);
			}
		}

		void blurForwardCumulative(float* output, int length, int strength)
		{
			if (output == nullptr || length <= 0 || strength <= 0) return;

			for (int i = 0; i < length; ++i)
			{
				float sum = 0.0f;
				for (int x = -strength; x <= 0; ++x)
					sum += getSampleSafe(output, length, i + x);

				output[i] = sum / (strength + 1);
			}
		}

		void blurForward(float* output, int length, int strength)
		{
			if (output == nullptr || length <= 0 || strength <= 0) return;

			std::vector<float> pristine;
			pristine.resize(length);
			memcpy(pristine.data(), output, length * sizeof(float));

			for (int i = 0; i < length; ++i)
			{
				float sum = 0.0f;
				for (int x = -strength; x <= 0; ++x)
					sum += getSampleSafe(pristine.data(), length, i + x);

				output[i] = sum / (strength + 1);
			}
		}
	} // namespace envelope
} // namespace exco