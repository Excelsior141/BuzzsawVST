#pragma once

namespace exco
{
	namespace math
	{
		// Translates to 3.141592653589793116 for some reason (drop of precision most likely)
		const static double kPId = 3.141592653589793238;

		// Not using the constant above with hopes of better precision
		const static double k2PId = 3.141592653589793238 * 2.0;

		// Float translations of double precision pi
		const static float kPIf = kPId;
		const static float k2PIf = k2PId;
	} // namespace math
} // namespace exco